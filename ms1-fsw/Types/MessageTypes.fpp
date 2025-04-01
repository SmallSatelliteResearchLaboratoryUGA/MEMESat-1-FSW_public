module MessageModule { 

    @ The maximum number of characters in a BBS message's message content text field 
    @ including a compulsory null-terminator at the end of the string.
    @
    @ I.e. this is the maximum length of a char buffer that needs to hold a BBS message's text content. 
    @
    @ When creating a BBSMessage from a c string, this maximum length and placing the null-terminator is automatically
    @ enforced. I.e., the string is truncated if necessary.
    constant Message_MaxCStrLength = 256 

    @ The maximum number of characters in a BBS message's message content text field 
    @ excluding the compulsory null-terminator at the end of the string.
    @
    @ I.e. this is the maximum number of characters that can be used for text content in a BBS message.
    constant Message_MaxTextLength = Message_MaxCStrLength - 1 

    @ The maximum number of messages that can be stored in a BBSMessage_Batch.
    @ This is a constant because it is used to define the size of the array of BBSMessages in a BBSMessage_Batch.
    @
    @ Components throughout the BBS Message system can (and currently do) refer to this as the maximum number of messages 
    @ that can be / are loaded and downlinked at once.
    constant Message_Batch_Size = 30
    
    @ An array of BBSMessages to be used in a BBSMessage_Batch. Not supposed to be used outside / without a 
    @ BBSMessage_Batch.
    array Message_Array = [Message_Batch_Size] Message

    @ A set of multiple BBSMessages that are sent together. 
    @
    @ For instance, this datatype can be used to load or pass multiple messages around the satellite at once.
    @ Note that is not suitable for use outside of the satellite becasuse BBSMessages are currently only implemented
    @ as pointers to dynamic heap memory where the actual message's content is stored.
    @
    @ A batch has a fixed maximum capacity of messages (BBSMessage_Batch_Size).
    @ The number of valid messages in the batch is stored in 'numValidMessages'. It is always smaller equals
    @ BBSMessage_Batch_Size.
    @ The first 'numValidMessages' entries in the 'messages' array are valid and contain the messages present in 
    @ this batch. 
    struct Message_Batch {
        @ The number of BBSMessages, each of which is one entry in the messages array, that are
        @ present in this batch.
        numValidMessages: U8 \
        format "{} messages" 

        messages: Message_Array @< The messages in this batch. The first 'numValidMessages' entries are valid.

    } default { numValidMessages = 0 }

    @ The data type in which a BBS message is handled for storing it on the satellite.
    @
    @ Note that the representation of the message on the satellite (i.e. just a fixed-size string of length 256) can be 
    @ very different from a HAM radio-user's or the ground station's perspective (e.g. in a JSON file format). 
    struct Message {
        @ The BBS message's text content.
        @
        @ When creating a BBSMessage from a c string, its maximum length and placing a null-terminator at the end
        @ is automatically enforced. I.e., the string is truncated if necessary. 
        @ The maximum number of characters (excl. the null-terminator) for the message_content is thus 
        @ BBSMessage_MaxCStrLength - 1.
        @
        @ The message_content may contain any byte values as characters expect for the null-terminator (0x00).
        @ The null-terminator is reserved for marking the end of message_content string in a char buffer.
        message_content: string size Message_MaxCStrLength 
    }

}