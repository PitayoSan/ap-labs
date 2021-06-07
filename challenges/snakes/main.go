package main

import (
	"fmt"
	_ "image/png"
	"log"
	"os"
	"scripts/scripts"
	"strconv"

	"github.com/hajimehoshi/ebiten"
)

var game scripts.Game

func init() {
	if len(os.Args) != 3 {
		fmt.Printf("2 arguments are needed to work. \n " +
			"        - Usage: go run main <food> <enemies> \n")
		os.Exit(1)
	}
	nFood, err := strconv.Atoi(os.Args[1])
	if err != nil || nFood <= 0 || nFood >= 957{
		fmt.Printf("'%s' is a invalid argument for number of food, try with a different number. \n", os.Args[1])
		os.Exit(1)
	}
	nEnemies, err2 := strconv.Atoi(os.Args[2])
	if err2 != nil || nEnemies <= 0 || nEnemies > 15{
		fmt.Printf("'%s' is a invalid argument for number of enemies, try with a different number. \n", os.Args[2])
		os.Exit(1)
	}

	game = scripts.NewGame(nFood, nEnemies)

}

// Game interface of ebiten
type Game struct{}

// Update the main thread of the game
func (g *Game) Update(screen *ebiten.Image) error {
	if err := game.Update(); err != nil {
		return err
	}
	return nil
}

// Draw renders the image in the window every tick
func (g *Game) Draw(screen *ebiten.Image) {
	if err := game.Draw(screen); err != nil {
		fmt.Println(err)
	}
}

// Layout : Function which adjust the screen width and height
func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 600, 700
}

func main() {
	ebiten.SetWindowSize(600, 700)
	ebiten.SetWindowTitle("SNEK   & <:========")

	if err := ebiten.RunGame(&Game{}); err != nil {
		log.Fatal(err)
	}
}
