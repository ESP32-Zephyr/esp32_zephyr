package main

import (
    "flag"
    "time"
    "fmt"
    "net"
    "os"

    "google.golang.org/protobuf/proto"
    "esp32_client/pb"
)

func usage() {
    fmt.Println("Missing valid host:port to connect to")
    fmt.Println("Example Usage: ")
    fmt.Printf("%s --dest_addr 192.168.1.12:4242\n",  os.Args[0])
}

func main() {
    destAddr := flag.String("dest_addr", "", "ESP32 IPv4 address & port. ipv4:port")
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

    cmd_id := pb.CommandId_VERSION_GET
    req := &pb.Request {
            Hdr: &pb.ReqHdr{
                Id: &cmd_id,
            },
            Pl: &pb.Request_VersionGet{
                VersionGet: &pb.VersionGetReq{},
            },
        }

    // Serialize to bytes
    req_raw, err := proto.Marshal(req)
    if err != nil {
        fmt.Println("Failed to marshal: %v", err)
    }

    fmt.Print("---> Sending: \n", req)
    _, err = sock.Write(req_raw)
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
        }
    }

    var res pb.Response
    if err := proto.Unmarshal(buffer[0:n], &res); err != nil {
        fmt.Println("Failed to unmarshal: %v", err)
    }
    fmt.Println("Response:", res)
    // Print the data read from the connection to the terminal
    //fmt.Println("<--- Received: ", string(buffer[0:n]))

}
