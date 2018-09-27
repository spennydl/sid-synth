int p_data = 13;
int p_shift_clk = 12;
int p_store_clk = 11;
int p_reset = 10;

void setup()
{
    pinMode(p_data, OUTPUT);
    pinMode(p_shift_clk, OUTPUT);
    pinMode(p_store_clk, OUTPUT);
    pinMode(p_reset, OUTPUT);

    digitalWrite(p_data, LOW);
    digitalWrite(p_shift_clk, LOW);
    digitalWrite(p_store_clk, LOW);
    digitalWrite(p_reset, LOW);

    delay(200);
    digitalWrite(p_reset, HIGH);

    digitalWrite(p_shift_clk, HIGH);
    delay(20);
    digitalWrite(p_shift_clk, LOW);
}

uint8_t data = 0xBB;
void loop()
{
    uint8_t d = data;
    for (int i = 0; i < 8; i++) {
        uint8_t bit = d & 0x01;
        d >>= 1;

        digitalWrite(p_data, bit); // data on the line
        //delay(20);
        digitalWrite(p_store_clk, HIGH); //store it
        //delay(20);
        digitalWrite(p_store_clk, LOW); //stop store
        //delay(20);
        digitalWrite(p_shift_clk, HIGH); //shift it
        //delay(20);
        digitalWrite(p_shift_clk, LOW); //stop shift
        //delay(20);

        digitalWrite(p_data, LOW); //reset data pin
    }

    delay(1000);

    data++;
}
