// websockets.go
package main

import (
	"bufio"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"log"
	"math"
	"net/http"
	"strconv"

	"github.com/gorilla/websocket"
	"github.com/tarm/serial"
)

func start_serial() {
	config := &serial.Config{
		Name:        "/dev/ttyUSB0",
		Baud:        9600,
		ReadTimeout: 1,
		Size:        8,
	}

	stream, err := serial.OpenPort(config)
	if err != nil {
		log.Fatal(err)
	}
	for {
		scanner := bufio.NewScanner(stream)

		for scanner.Scan() {
			buf := make([]byte, 1024)
			buf = scanner.Bytes()
			n := len(buf)
			if 'E' == buf[n-1] && 'S' == buf[0] {
				fmt.Println(buf)
				fmt.Println(math.Float32frombits(binary.LittleEndian.Uint32(buf[1:])))
				fmt.Println(math.Float32frombits(binary.LittleEndian.Uint32(buf[5:])))
			} else {
				fmt.Println(scanner.Text())
			}

			/*
				struct DataFrame {
					float f1;
					float f2;
					unsigned long timestamp;
					unsigned long spins;
					float rpms;
					char status = 'o';
				};
			*/
		}
		if err := scanner.Err(); err != nil {
			log.Fatal(err)
		}
	}
}

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

func main() {

	type DataPoint struct {
		ID     int
		Torque float64
	}

	http.HandleFunc("/echo", func(w http.ResponseWriter, r *http.Request) {
		conn, _ := upgrader.Upgrade(w, r, nil) // error ignored for sake of simplicity

		for {
			// Read message from browser
			msgType, InMsg, err := conn.ReadMessage()
			if err != nil {
				return
			}

			f, err := strconv.ParseFloat(string(InMsg), 64)
			dp := DataPoint{
				ID:     1,
				Torque: f,
			}

			OuMsg, err := json.Marshal(dp)
			if err != nil {
				fmt.Println("error:", err)
			}

			// Print the message to the console
			fmt.Printf("%s sent: %s\n", conn.RemoteAddr(), string(OuMsg))

			// Write message back to browser
			if err = conn.WriteMessage(msgType, OuMsg); err != nil {
				return
			}
		}
	})

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		http.ServeFile(w, r, "websockets.html")
	})

	go http.ListenAndServe(":8080", nil)

	start_serial()
}

// package main

// //https://gowebexamples.com/websockets/

// import (
// 	"fmt"
// 	"net/http"
// 	//	"github.com/tarm/serial"
// )

// func hello(w http.ResponseWriter, req *http.Request) {

// 	fmt.Fprintf(w, "hello\n")
// }

// func headers(w http.ResponseWriter, req *http.Request) {

// 	for name, headers := range req.Header {
// 		for _, h := range headers {
// 			fmt.Fprintf(w, "%v: %v\n", name, h)
// 		}
// 	}
// }

// func main() {

// 	http.HandleFunc("/hello", hello)
// 	http.HandleFunc("/headers", headers)

// 	// c := &serial.Config{Name: "/dev/ttyUSB0", Baud: 9600}
// 	// s, err := serial.OpenPort(c)
// 	// if err != nil {
// 	// 	log.Fatal(err)
// 	// }

// 	// n, err := s.Write([]byte("test"))
// 	// if err != nil {
// 	// 	log.Fatal(err)
// 	// }

// 	// buf := make([]byte, 128)
// 	// n, err = s.Read(buf)
// 	// if err != nil {
// 	// 	log.Fatal(err)
// 	// }
// 	//log.Print("%q", buf[:n])

// 	http.ListenAndServe(":8090", nil)
// }
