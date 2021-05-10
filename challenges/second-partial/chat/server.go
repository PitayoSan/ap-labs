// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"errors"
	"flag"
	"fmt"
	"log"
	"net"
	"strings"
	"time"
)

// Structs declaration
type Client struct {
	username string
	ip string
	port string
	admin bool
	channel clientChan
	lastLogin time.Time
	conn net.Conn
	banned bool
}

type Message struct {
	author string
	message string
	server bool
}

type ConnMessage struct {
	message string
	mtype int // 0: echo, 1: normal message, 2: kicked message
}

// Channel declaration
type clientChan chan<- ConnMessage // an outgoing message channel

var (
	entering = make(chan *Client) // clients entering the chat
	leaving  = make(chan *Client) // clientes leaving the chat
	messages = make(chan Message) // all incoming client messages
)

// Messages

// - Helpers

// Checks if params >= to the specification
func checkMinParameters(msg string, params int) ([]string, error) {
	if msgComponents := strings.Split(msg, " "); len(msgComponents) >= params {
		return msgComponents, nil
	}
	return nil, errors.New("not enough parameters")
}

// Checks if params == to the specification
func checkParameters(msg string, params int) ([]string, error) {
	if msgComponents := strings.Split(msg, " "); len(msgComponents) == params {
		return msgComponents, nil
	}
	return nil, errors.New("too much/not enough parameters")
}

// Adds a prefix to any text
func formatPrefix(prefix string, text string) string {
	return fmt.Sprintf("%s > %s", prefix, text)
}

// Adds the system prefix to any text
func formatServerPrefix(text string) string {
	return formatPrefix("irc-server", text)
}

// Prints text with the system prefix to the server's terminal
func printToCLI(text string) {
	fmt.Println(formatServerPrefix(text))
}

// - Handling and broadcasting

// Polls channels and acts on the trasmitted elemetents
func broadcaster() {
	// All connected clients
	clients := make(map[string]*Client)
	for {
		select {
		// Handles all messages directed to the users
		case msg := <-messages:
			handleMessage(msg, &clients)

		// Handles incoming users
		case cli := <-entering:
			if len(clients) == 0 {
				(*cli).admin = true
				(*cli).channel <- ConnMessage{
					message: formatServerPrefix("Congrats, you were the first user."),
					mtype: 1,
				}
				greetAdmin(*cli)
			}
			clients[(*cli).username] = cli

		// Handles outcoming users
		case cli := <-leaving:
			delete(clients, (*cli).username)
			close((*cli).channel)

			// Changes administration of the channel
			if (*cli).admin {
				if len(clients) > 0 {
					var earliestcli *Client
					earliestTime := time.Now()
					for _, ncli := range clients {
						if (*ncli).lastLogin.Before(earliestTime) || (*ncli).lastLogin.Equal(earliestTime) {
							earliestcli = ncli
							earliestTime = (*ncli).lastLogin
						}
					}
					(*earliestcli).admin = true
					greetAdmin((*earliestcli))
				}
			}
		}
	}
}

// Handles each message according to its contents
func handleMessage(msg Message, clients *map[string]*Client) {

	// Only broadcasts server messages
	if msg.server {
		serverBroadcast(msg.message, clients)
		return
	}

	currentcli := (*(*clients)[msg.author])

	switch {
	// Gets info from all users
	case strings.Index(msg.message, "/users") == 0:
		getUsers(msg, clients)

	// Sends a private message to other user
	case strings.Index(msg.message, "/msg") == 0:
		msgComponents, err := checkMinParameters(msg.message, 3)
		if err != nil {
			currentcli.channel <- ConnMessage{
				message: "usage: /msg <user> <msg>",
				mtype: 1,
			}
			break
		}

		dest := msgComponents[1]
		destcli, exists := (*clients)[dest]
		if !exists {
			currentcli.channel <- ConnMessage{
				message: "user [" + dest + "] is not connected",
				mtype: 1,
			}
			break
		}

		sendDirectMessage(strings.Join(msgComponents[2:], " "), currentcli, *destcli)

	// Tells the localtime of the server
	case strings.Index(msg.message, "/time") == 0:
		getTime(currentcli)

	// Gets info from a specific user
	case strings.Index(msg.message, "/user") == 0:
		msgComponents, err := checkParameters(msg.message, 2)
		if err != nil {
			currentcli.channel <- ConnMessage{
				message: "usage: /user <user>",
				mtype: 1,
			}
			break
		}

		user := msgComponents[1]
		usercli, exists := (*clients)[user]
		if !exists {
			currentcli.channel <- ConnMessage{
				message: "user [" + user + "] is not connected",
				mtype: 1,
			}
			break
		}

		getUser(currentcli, *usercli)

	// [Admin only] kicks an user from the channel
	case strings.Index(msg.message, "/kick") == 0:
		msgComponents, err := checkParameters(msg.message, 2)
		if err != nil || !currentcli.admin {
			currentcli.channel <- ConnMessage{
				message: "[ADMIN] usage: /kick <user>",
				mtype: 1,
			}
			break
		}

		kickedUser := msgComponents[1]
		kickedcli, exists := (*clients)[kickedUser]
		if !exists {
			currentcli.channel <- ConnMessage{
				message: "user [" + kickedUser + "] is not connected",
				mtype: 1,
			}
			break
		}

		kickUser(kickedcli, clients)
	
	default:
		broadcastMessage(msg, clients)
	}	
}

// Iterates the clients and sends the requested info
func getUsers(msg Message, clients *map[string]*Client) {
	ch := (*(*clients)[msg.author]).channel
	for username, cli := range *clients {
		msg := fmt.Sprintf(
			"[%s] - connected since " + (*cli).lastLogin.Format("2006-01-02 15:04:05"),
			username,
		)
		ch <- ConnMessage{
			message: formatServerPrefix(msg),
			mtype: 1,
		}
	}
}

//  Just sends the message to the specified user
func sendDirectMessage(msg string, sender Client, dest Client) {
	sender.channel <- ConnMessage{mtype: 0}
	dest.channel <- ConnMessage{
		message: formatPrefix(sender.username + " (private)", msg),
		mtype: 1,
	}
}

// Gets local time and zone of the server and sends them
func getTime(cli Client) {
	localtime := time.Now()
	location, _ := time.LoadLocation("America/Mexico_City")
	msg := fmt.Sprintf(
		"Local Time: %s " + localtime.In(location).Format("15:04"),
		location,
	)
	cli.channel <- ConnMessage{
		message: formatServerPrefix(msg),
		mtype: 1,
	}
}

// Gets the requested info from the specified user
func getUser(clidest Client, clinfo Client) {
	msg := fmt.Sprintf(
		"username: %s, IP: %s Connected since: " + clinfo.lastLogin.Format("2006-01-02 15:04:05"),
		clinfo.username,
		clinfo.ip,
	)
	clidest.channel <- ConnMessage{
		message: formatServerPrefix(msg),
		mtype: 1,
	}
}

// Kicks user, and alerts kicked user, the server terminal and the other users
func kickUser(kcli *Client, clients *map[string]*Client) {
	(*kcli).banned = true
	(*kcli).channel <- ConnMessage{
		message: formatServerPrefix("You were kicked from this channel"),
		mtype: 2,
	}

	time.Sleep(1 * time.Second)
	(*kcli).conn.Close()
	delete(*clients, (*kcli).username)
	close((*kcli).channel)

	serverMsg := fmt.Sprintf(
		"[%s] was kicked",
		(*kcli).username,
	)
	printToCLI(serverMsg)
	serverBroadcast(serverMsg, clients)
}

// Broadcasts a server message to all users
func serverBroadcast(msg string, clients *map[string]*Client) {
	for _, cli := range *clients {
		(*cli).channel <- ConnMessage{
			message: msg,
			mtype: 1,
		}
	}
}

// Broadcasts a message to all users except sender
func broadcastMessage(msg Message, clients *map[string]*Client) {
	(*(*clients)[msg.author]).channel <- ConnMessage{mtype: 0}
	if msg.message != "" {
		for username, cli := range *clients {
			if username != msg.author {
				(*cli).channel <- ConnMessage{
					message: formatPrefix(msg.author, msg.message),
					mtype: 1,
				}
			}
		}
	}
}

// User management

// Adds new user, and alerts server terminal and all users
func logNewUser(addrcli *Client) {
	cli := *addrcli
	newUserText := fmt.Sprintf(
		"New connected user [%s]",
		cli.username,
	)
	printToCLI(newUserText)
	messages <- Message {
		message: formatServerPrefix(newUserText),
		server: true,
	}
	cli.channel <- ConnMessage{
		message: formatServerPrefix("Welcome to the Simple IRC Server"),
		mtype: 1,
	}
	cli.channel <- ConnMessage{
		message: formatServerPrefix(fmt.Sprintf(
			"Your user [%s] is successfully logged",
			cli.username,
		)),
		mtype: 1,
	}

	entering <- addrcli
}

// Greets the first admin of the channel
func greetAdmin(cli Client) {
	printToCLI(fmt.Sprintf(
		"[%s] was promoted as the channel ADMIN",
		cli.username,
	))
	cli.channel <- ConnMessage{
		message: formatServerPrefix("You're the new IRC Server ADMIN"),
		mtype: 1,
	}
}

// Removes user that left and alerts the other users
func userLeft(addrcli *Client) {
	cli := *addrcli
	leaving <- addrcli
	printToCLI(fmt.Sprintf(
		"[%s] left",
		cli.username,
	))
	messages <- Message {
		message: formatServerPrefix(fmt.Sprintf(
			"[%s] left channel",
			cli.username,
		)),
		server: true,
	}
}

// Handle connection

// Receives info from the user, and orchestrates the receival of its messages
func handleConn(conn net.Conn) {
	ch := make(chan ConnMessage) // outgoing client messages

	buffer := make([]byte, 1024)
	readBytes, err := conn.Read(buffer)
	if err != nil {
		log.Fatal("couldn't otbain username")
	}

	username := string(buffer[:readBytes])
	address := strings.Split(conn.RemoteAddr().String(), ":")

	go clientWriter(conn, ch, username)

	user := Client {
		username: username,
		ip: address[0],
		port: address[1],
		admin: false,
		channel: ch,
		lastLogin: time.Now(),
		conn: conn,
		banned: false,
	}

	logNewUser(&user)

	input := bufio.NewScanner(conn)
	for input.Scan() {
		messages <- Message {
			author: username,
			message: input.Text(),
		}
	}

	/*
	Ignoring errors as the usual produced error is:
	> read tcp x.x.x.x:xxxxx->x.x.x.x:xxxxx use of closed network connection
	which is expected when a connection with a user is closed

	if err := input.Err(); err != nil {
		fmt.Println(err)
	}
	*/

	if !user.banned {
		userLeft(&user)
	}
}

// Writes all produces messages to the respective channel of all users
func clientWriter(conn net.Conn, ch <-chan ConnMessage, username string) {
	for msg := range ch {
		var message string
		count := len(username + " > ") - len(msg.message)
		if count < 0 {
			count = 0
		}
		refill := strings.Repeat(" ", count)
		switch msg.mtype {
			// Echo
			case 0:
				message =  username + " > "
		
			// Usual message
			case 1:
				message =  "\r" + msg.message + refill + "\n" + username + " > "
		
			// Last message for the user
			case 2:
				message =  "\r" + msg.message + "\n"
			}
		fmt.Fprint(conn, message)

		/*
		Ignoring errors as the usual produced error is:
		> write tcp x.x.x.x:xxxxx->x.x.x.x:xxxxx use of closed network connection
		which is expected when a connection with a user is closed

		if err != nil {
			fmt.Println(err)
		}
		*/
	}
}

// Main
func main() {

	// Parsing CLI arguments
	noHost := "-1"
	noPort := -1
	var hostFlag string
	var portFlag int
	flag.StringVar(&hostFlag, "host", noHost, "host to connect to")
	flag.IntVar(&portFlag, "port", noPort, "port to connect to")

	flag.Parse()

	if hostFlag == noHost {
		fmt.Println("-host must be assigned")
		fmt.Println(hostFlag)
		return
	}

	if portFlag == noPort {
		fmt.Println("-port must be assigned")
		return
	}

	server := fmt.Sprintf("%s:%d", hostFlag, portFlag)

	// Init server
	printToCLI(fmt.Sprintf(
		"Simple IRC Server started at %s",
		server,
	))
	printToCLI("Ready for receiving new clients")

	listener, err := net.Listen("tcp", server)
	if err != nil {
		log.Fatal(err)
	}

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}
