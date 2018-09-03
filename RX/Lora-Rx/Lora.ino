
/*
                                                           ,-----.                 
                                                    |     |                    
                                                    ".___."                 
                                                 ______|_____                       
                                                |,----------.|            
                                                ||          ||            
                                                ||   LORA   ||                  
                                                ||          ||                      
                                                |`----------'|     ,--|=             
                                                |o .... [###]|\_,--'  hjw            
                                                |____________|        `97
    

 *      ---------------------------------SENDING STRUCT OVER LORA---------------------------------------------- 
 *     Author:Marco Manfrè
*      Version:2.0
*      
*      Description:
 *     The theory is that a larger struct corresponds to a length value> RH_RF95_MAX_MESSAGE_LEN
*      RH_RF95_MAX_MESSAGE_LEN is a value integrated into the radio driver, it can not be changed.
 *     A solution could be to divide the message into compatible submultiples and send them one after the other 
*/
#define MAX RH_RF95_MAX_MESSAGE_LEN
template <typename T> unsigned int lora_write (const T& value)
  {
  
  if(rf95.send((uint8_t *) &value, sizeof (value)))//the message is ok,send them as it is
  return sizeof (value);
  else {
    //the message is too big,it must be divided
    uint8_t split_op;//i use this variable for storing the number of sub-message i must send
    split_op  = sizeof(value)%RH_RF95_MAX_MESSAGE_LEN;
    split_op += sizeof(value)/RH_RF95_MAX_MESSAGE_LEN;//count the number of packets 
    T split[split_op];//create an array 
    uint8_t i;
    uint8_t * p = (byte*) &value;//store the head address of my structure
    for(i=0;i<split_op;i++){
      memcpy(&split[i],p,RH_RF95_MAX_MESSAGE_LEN);//divide the content of main structure in the array fragments 
      p=p+RH_RF95_MAX_MESSAGE_LEN;
    }
    i=0;
    for(i=0;i<split_op;i++){
      if(!rf95.send((uint8_t *) &split[i],sizeof(split)))//now send the fragments
        return 1000+i;
    }
  }
  
} 
template <typename T> unsigned int lora_read(T& value)
  {
    //even if the message recived is fragmented the function waits until the structure is complete
    
    uint8_t * p = (byte*) &value;
    uint8_t  len =sizeof(value);
    unsigned int i;
    for (i = 0; i < sizeof value; i++){
       
              rf95.recv( p ,&len);
              p=p+1;
        
         }
    return i;
} 
