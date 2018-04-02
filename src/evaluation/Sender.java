import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.util.HashSet;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.net.InetAddress;

class Sender {


    public static void main(String args[]) throws Exception {
        // print beginning date/time
        System.out.println("Starting sender at: " + System.currentTimeMillis());

        String ip = args[0]; // IP address to send packet to
        int portNo = Integer.parseInt(args[1]); // port to send packet to
        int maxPacket = Integer.parseInt(args[2]); // number of packets to send
        InetAddress host = InetAddress.getByName(ip);
        DatagramSocket s = new DatagramSocket();

        /* The first two commands need to be run only once */
        int count = 0;
        while (count != maxPacket) {
            if (count % 100 == 0) {
                System.out.println("Sent " + count + " packets");
            }
            count++;
            byte[] data = "sample data".getBytes();
            DatagramPacket packet = new DatagramPacket(data, data.length, host, portNo);
            s.send(packet);
        }

        System.out.println("Finished sender at: " + System.currentTimeMillis());

    }
}