package main

import (
    "flag"
    "time"
    "fmt"
    "net"
    "os"
)

func usage() {
    fmt.Println("Missing valid host:port to connect to")
    fmt.Println("Example Usage: ")
    fmt.Printf("%s --dest_addr 192.168.1.12:4242\n",  os.Args[0])
}

func main() {
    destAddr := flag.String("dest_addr", "", "ESP32 IPv4 address & port. ipv4:port")
    msgCnt := flag.Int("msg_cnt", 3, "Number of test messages to send")
    flag.Parse()

    if *destAddr == "" {
        usage()
        os.Exit(1)
    }
    // Resolve the string address to a UDP address
    esp32Addr, err := net.ResolveUDPAddr("udp", *destAddr)
    if err != nil {
        fmt.Println(err)
        os.Exit(1)
    }
    // Dial to the address with UDP
    sock, err := net.DialUDP("udp", nil, esp32Addr)
    if err != nil {
        fmt.Println(err)
        os.Exit(1)
    }
     defer sock.Close()

    for i := 1; i <= *msgCnt; i++ {
        testMsg := fmt.Sprintf("Test message: %d\n", i)

        _, err = sock.Write([]byte(testMsg))
        fmt.Print("---> Sending: ", testMsg)
        if err != nil {
            fmt.Println(err)
            os.Exit(1)
        }

        buffer := make([]byte, 1024)
        sock.SetReadDeadline(time.Now().Add(2 * time.Second))
        n, _, err := sock.ReadFromUDP(buffer)
        if err != nil {
            if e, ok := err.(net.Error); !ok || !e.Timeout() {
                fmt.Println(err)
                os.Exit(1)
            } else {
                fmt.Println("---| Receive timeout")
                continue
            }
        }

        // Print the data read from the connection to the terminal
        fmt.Print("<--- Received: ", string(buffer[0:n]))
    }
}
