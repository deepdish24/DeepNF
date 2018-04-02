import java.util.HashSet;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.io.*;
import java.net.*;

class Sender {


    public static void main(String args[]) throws Exception {
        // print beginning date/time
        System.out.println("Starting sender at: " + System.currentTimeMillis());

        String ip = args[0]; // IP address to send packet to
        int portNo = Integer.parseInt(args[1]); // port to send packet to
        int maxPacket = Integer.parseInt(args[2]); // number of packets to send

        Socket socket = new Socket (ip, portNo);
        PrintWriter out = new PrintWriter (socket.getOutputStream(), true);

        /* The first two commands need to be run only once */
        int count = 0;
        while (count != maxPacket) {
            if (count % 100 == 0) {
                System.out.println("Sent " + count + " packets");
            }
            count++;

            out.println ("\n");
            out.flush();
        }

        System.out.println("Finished sender at: " + System.currentTimeMillis());

    }
}