package bgu.spl.net.impl.stomp;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;

import bgu.spl.net.api.MessageEncoderDecoder;

public class StompFrameEncoderDecoder implements MessageEncoderDecoder<String>  {
     private final ArrayList<Byte> buffer = new ArrayList<>();

    @Override
    public String decodeNextByte(byte nextByte) {
        if (nextByte == '\0') { // Null character indicates end of frame
            byte[] bytes = new byte[buffer.size()];
            for (int i = 0; i < buffer.size(); i++) {
                bytes[i] = buffer.get(i);
            }
            buffer.clear();
            return new String(bytes, StandardCharsets.UTF_8);
        } else {
            buffer.add(nextByte);
            return null; // Frame is incomplete
        }
    }

    @Override
    public byte[] encode(String message) {
        return (message + '\0').getBytes(StandardCharsets.UTF_8);
    }
}
