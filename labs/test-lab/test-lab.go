package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
  if len(os.Args) > 1 {
    names := strings.Join(os.Args[1:], " ")
    greeting := fmt.Sprintf("Hello %s, Welcome to the Jungle", names)
    fmt.Println(greeting)
  } else {
    fmt.Println("No name was entered!")
  }
}
