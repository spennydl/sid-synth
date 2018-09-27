use std::fs::File;
use std::io::BufReader;
use std::io::Error;
use std::string::String;
use std::io::prelude::*;

mod lex;

use lex::Token;
use lex::LexError;
use lex::Lexer;

fn run(name: String) -> Result<(), Error> {
    let file = File::open(name)?;
    let mut rdr = BufReader::new(file);

    let mut lex = Lexer::new(rdr);

    match lex.tokenize() {
        Ok(tokens) => {
            println!("{:?}", tokens);
            Ok(())
        },
        Err(e) => {
            println!("err: {:?}", e);
            Ok(())
        }
    }
}

fn main() {
    match run("./song.a".to_string()) {
        Ok(_) => println!("done"),
        Err(_) => println!("err")
    } 
}
