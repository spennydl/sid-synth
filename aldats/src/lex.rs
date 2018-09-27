use std::string::String;
use std::error::Error;
use std::io::BufReader;
use std::io::Read;
use std::io::BufRead;
use std::iter::Peekable;
use std::iter::Iterator;
use std::fmt;
use std::cell::RefCell;

/// Wrapper around a `Read` that reads `char`s.
#[derive(Debug)]
struct CharBufReader<R> {
    reader: BufReader<R>,
    line: String,
    linepos: usize,
    linelen: usize
}

impl<R: Read> CharBufReader<R> {
    fn new(r: R) -> CharBufReader<R> {
        CharBufReader {
            reader: BufReader::new(r),
            line: String::new(),
            linepos: 0,
            linelen: 0
        }
    }
}

impl<R: Read> Iterator for CharBufReader<R> {
    type Item = char;

    fn next(&mut self) -> Option<char> {
        if self.linelen == 0 || self.linepos == self.linelen {
            self.line.clear();

            match self.reader.read_line(&mut self.line) {
                Ok(sz) => self.linelen = sz,
                Err(_) => return None
            }

            self.linepos = 0;
        }
        
        let next = self.line.chars().nth(self.linepos);
        self.linepos += 1;

        next
    }
}

/// Represents an alda token as read by the lexer.
#[derive(Debug)]
pub enum Token {
    OpenParens,
    CloseParens,
    Identifier(String),
    StringValue(String),
    NumericValue(i32),
    StringToken(String)
}

#[derive(Debug, PartialEq, Eq)]
enum LexContext {
    Score,
    //Voice, -- TODO: parse voice definitions
    InParens
}

#[derive(Debug)]
pub struct Lexer<R> where R: Read {
    raw: RefCell<Peekable<CharBufReader<R>>>,
    ctx: RefCell<LexContext>,
    last_ctx: RefCell<LexContext>,
    line: u32,
    col: u32,
}

#[derive(Debug)]
pub struct LexError {
    msg: String
}

impl Error for LexError {
    fn description(&self) -> &str {
        "Lexing error"
    }
}

impl fmt::Display for LexError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "LexError - {}", self.msg)
    }
}

impl LexError {
    fn new(s: &str) -> LexError {
        LexError { msg: String::from(s) }
    }
}

impl<R: Read> Lexer<R> {
    pub fn new(from: R) -> Lexer<R> {
        Lexer {
            raw: RefCell::new(CharBufReader::new(from).peekable()),
            ctx: RefCell::new(LexContext::Score),
            last_ctx: RefCell::new(LexContext::Score),
            line: 0,
            col: 0
        }
    }

    fn set_ctx(&self, n_ctx: LexContext) {
        self.last_ctx.swap(&self.ctx);
        self.ctx.replace(n_ctx);
    }

    fn restore_last_ctx(&self) {
        self.ctx.swap(&self.last_ctx);
    }

    fn ctx_is(&self, other: LexContext) -> bool {
        *self.ctx.borrow() == other
    }

    pub fn tokenize(mut self) -> Result<Vec<Token>, LexError> {
        let mut result: Vec<Token> = Vec::new();

        loop {
            match self.peek_next() {
                Some('(') => {
                    result.push(self.read_parens()?);
                    self.set_ctx(LexContext::InParens);
                },
                Some(')') => {
                    result.push(self.read_parens()?);
                    self.restore_last_ctx();
                },

                Some(c) if c.is_numeric() => {
                    let n = self.read_number()?;
                    result.push(Token::NumericValue(n));
                },

                Some('\n') => {
                    self.line = self.line + 1;
                    self.advance();
                }

                Some(c) if self.is_delim(c) => {
                    self.advance();
                },

                Some(_) if self.ctx_is(LexContext::Score) => {
                    result.push(self.read_string_token()?);
                },

                Some(_) if self.ctx_is(LexContext::InParens) => {
                    // we will only support key-value attribute settings in parens atm
                    result.push(self.read_identifier()?);
                    self.advance();
                    result.push(self.read_value()?);
                },

                Some(_) => {
                    return Err(LexError::new("Found unmatched char"));
                }
                None => {
                    break;
                }
            }
        }
        Ok(result)
    }

    fn peek_next(&self) -> Option<char> {
        self.raw.borrow_mut().peek().map(|&c| c)
    }

    fn is_delim(&self, c: char) -> bool {
        c.is_whitespace() || c == '(' || c == ')'
    }

    fn advance(&self) -> Option<char> {
        self.raw.borrow_mut().next()
    }

    fn read_delimed_chunk(&self) -> Result<String, LexError> {
        let mut tok: String = String::new();
        loop {
            match self.peek_next().expect("err unexpected char") {
                c if !(self.is_delim(c)) => tok.push(self.advance().unwrap()),
                _ => break
            }
        }
        Ok(tok)
    }

    fn read_number(&self) -> Result<i32, LexError> {
        let tok = self.read_delimed_chunk()?;

        match i32::from_str_radix(&tok, 10) {
            Ok(n) => Ok(n),
            Err(_) => Err(LexError::new("failed to parse number"))
        }
    }

    fn read_identifier(&self) -> Result<Token, LexError> {
        let tok = self.read_delimed_chunk()?;
        Ok(Token::Identifier(tok))
    }

    fn read_string_token(&self) -> Result<Token, LexError> {
        let tok = self.read_delimed_chunk()?;
        Ok(Token::StringToken(tok))
    }

    fn read_parens(&self) -> Result<Token, LexError> {
        match self.raw.borrow_mut().by_ref().next() {
            Some('(') => Ok(Token::OpenParens),
            Some(')') => Ok(Token::CloseParens),
            Some(c) => Err(LexError::new(&format!("LexError: unexpected char {}", c))),
            None => Err(LexError::new("LexErr: expected token"))
        }
    }

    fn read_value(&self) -> Result<Token, LexError> {
        let tok = self.read_delimed_chunk()?;

        match i32::from_str_radix(&tok, 10) {
            Ok(n) => Ok(Token::NumericValue(n)),
            Err(_) => Ok(Token::StringValue(tok))
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs::File;
    use std::io;

    fn make_char_reader(filename: &str) -> io::Result<CharBufReader<BufReader<File>>> {
        let mut file = File::open(filename)?;
        let mut buf_read = BufReader::new(file);
        let mut r = CharBufReader::new(buf_read);

        Ok(r)
    }

    #[test]
    fn test_char_reader() {
        let mut r = make_char_reader("test_char_reader.test").unwrap();
        let c = r.next().unwrap();

        assert_eq!(c, 'T');
    }

    #[test]
    fn test_char_reader_iter() {
        let mut reader = make_char_reader("test_char_reader_iter.test").unwrap();

        let s: String = reader.collect();

        assert_eq!(s, "it works!\neven with multiple lines\n");
    }

    #[test]
    fn test_lexer_peek() {
        let mut file = File::open("test_lexer_peek.test").unwrap();
        let mut lex = Lexer::new(file);

        let mut c = lex.peek_next().unwrap();

        assert_eq!(c, 'q');

        let c = lex.peek_next().unwrap();

        assert_eq!(c, 'q');
    }

}
