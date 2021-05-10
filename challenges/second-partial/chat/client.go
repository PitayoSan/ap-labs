// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

func main() {

	// Parsing CLI arguments
	noFlag := "-1"
	var userFlag, serverFlag string
	flag.StringVar(&userFlag, "user", noFlag, "name for current user")
	flag.StringVar(&serverFlag, "server", noFlag, "<ip:port> to connect to")

	flag.Parse()

	if userFlag == noFlag {
		fmt.Println("-user must be assigned")
		return
	}

	if serverFlag == noFlag {
		fmt.Println("-server must be assigned")
		return
	}
 
	conn, err := net.Dial("tcp", serverFlag)
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	go func() {
		_, err := io.Copy(os.Stdout, conn)
		if err != nil {
			log.Fatal(err)
		}
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	conn.Write([]byte(userFlag))
	mustCopy(conn, os.Stdin)

	conn.Close()
	<-done // wait for background goroutine to finish
	return
}

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
