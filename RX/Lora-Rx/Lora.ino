
/*La teoria è che una struct più grande corrisponde a un valore di lunghezza > RH_RF95_MAX_MESSAGE_LEN
 * RH_RF95_MAX_MESSAGE_LEN è un valore integrato al driver della radio,non può essere modificato
 * una soluzione potrebbe essere quella di dividere il messaggio in sottomultipli compatibili e inviarli in treno
 * 
*/
#define MAX RH_RF95_MAX_MESSAGE_LEN
template <typename T> unsigned int lora_write (const T& value)
  {
  
  if(rf95.send((uint8_t *) &value, sizeof (value)))//se la funzione restituisce falso,il messaggio è troppo grande
  return sizeof (value);
  else {
    //il messaggio è troppo grande devo dividerlo e inviarlo a pezzi
    uint8_t split_op;//uso questo numero per capire quanti messaggi devo inviare
    split_op  = sizeof(value)%RH_RF95_MAX_MESSAGE_LEN;
    split_op += sizeof(value)/RH_RF95_MAX_MESSAGE_LEN;
    T split[split_op];
    uint8_t i;
    uint8_t * p = (byte*) &value;
    for(i=0;i<split_op;i++){
      memcpy(&split[i],p,RH_RF95_MAX_MESSAGE_LEN);
      p=p+RH_RF95_MAX_MESSAGE_LEN;
    }
    i=0;
    for(i=0;i<split_op;i++){
      if(!rf95.send((uint8_t *) &split[i],sizeof(split)))
        return 1000+i;
    }
  }
  
} 
template <typename T> unsigned int lora_read(T& value)
  {
    //in teoria anche se il messaggio arriva a pezzi dovrebbe unirlo lo stesso
    
    uint8_t * p = (byte*) &value;// parte alta dell'indirizzo
    uint8_t  lunghezza =sizeof(value);
    unsigned int i;
    for (i = 0; i < sizeof value; i++){
       
              rf95.recv( p ,&lunghezza);
              p=p+1;
        
         }
    return i;
} 
