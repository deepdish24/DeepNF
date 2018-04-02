import java.util.HashSet;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.io.*;
import java.net.*;

class Receiver {

    static byte[] receiveData; // buffer for received UDP packets
    static DatagramSocket serverSocket;

    public static void main(String args[]) throws Exception {
        System.out.println("Receiver starting at: " + System.currentTimeMillis());

        int portNo = Integer.parseInt(args[0]); // port number to listen to packets on
        int maxPacket = Integer.parseInt(args[1]); // number of packets to listen for

        ServerSocket serverSocket = new ServerSocket (portNo);
        Socket clientSocket = serverSocket.accept ();

        BufferedReader in = new BufferedReader (new InputStreamReader (clientSocket.getInputStream ()));

        int count = 0;
        while (count != maxPacket ) {
            count++;
            if (count % 100 == 0) {
                System.out.println("Received " + count + " packets");
            }

            // receive a packet
            String fromClient = in.readLine ();

        }

        long endTime = System.nanoTime();

        System.out.println("Receiver ending at: " + System.currentTimeMillis());

        in.close ();
        clientSocket.close ();
        serverSocket.close ();


    }
}