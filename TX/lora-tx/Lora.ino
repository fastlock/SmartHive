template <typename T> unsigned int lora_write (const T& value)
  {
  rf95.send((byte *) &value, sizeof (value));
  return sizeof (value);
} 
template <typename T> unsigned int lora_read(T& value)
  {
    byte * p = (byte*) &value;
    uint8_t  lunghezza =sizeof(value);
    unsigned int i;
    for (i = 0; i < sizeof value; i++)
         
           rf95.recv( p ,&lunghezza);
              p=p+1;
    return i;
} 
