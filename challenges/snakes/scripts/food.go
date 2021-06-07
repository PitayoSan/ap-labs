package scripts

import (
	"math/rand"
	"time"

	"github.com/hajimehoshi/ebiten"
	"github.com/hajimehoshi/ebiten/ebitenutil"
)


type Food struct {
	x       float64
	y       float64
	eaten   bool
	foodImg ebiten.Image
}

// GenFood Here we generate the foods
func GenFood() *Food {
	food := Food{
		eaten:  false,
	}
	foodImg, _, _ := ebitenutil.NewImageFromFile("images/cherry.png", ebiten.FilterDefault)
	food.foodImg = *foodImg

	seed := rand.NewSource(time.Now().UnixNano())
	random := rand.New(seed)

	// We set random values for X and Y, so the food position is random, always inside the boundaries
	food.x = float64(random.Intn(28)*20 + 20)
	food.y = float64(random.Intn(30)*20 + 60)
	return &food
}


func (food *Food) Update(dotTime int) error {
	return nil
}

// Draw the foodImg in its random position
func (food *Food) Draw(screen *ebiten.Image, dotTime int) error {
	drawer := &ebiten.DrawImageOptions{}
	drawer.GeoM.Translate(food.x, food.y)
	screen.DrawImage(&food.foodImg, drawer)
	return nil
}
