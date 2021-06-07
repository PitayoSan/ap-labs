package scripts

import (
	"math/rand"
	"time"

	"github.com/hajimehoshi/ebiten"
	"github.com/hajimehoshi/ebiten/ebitenutil"
)


type EnemySnake struct {
	game             *Game
	nBodyP           int
	lastDir          string
	sHeadU           ebiten.Image
	sHeadD           ebiten.Image
	sHeadL           ebiten.Image
	sHeadR           ebiten.Image
	bodyE            ebiten.Image
	partsOfBody      [][]float64
	seed             rand.Source
	pointsWaiting    int
	score            int
	channelMovements chan int
	collision        bool
}


func CreateEnemySnake(g *Game) *EnemySnake {
	e := EnemySnake{
		game:          g,
		nBodyP:        0,
		lastDir:       "right",
		pointsWaiting: 0,
		collision:     false,
	}

	e.channelMovements = make(chan int)
	e.seed = rand.NewSource(time.Now().UnixNano())
	random := rand.New(e.seed)

	e.partsOfBody = append(e.partsOfBody, []float64{float64(random.Intn(30) * 20), float64(random.Intn(30) * 20)})

	sHeadU, _, _ := ebitenutil.NewImageFromFile("images/headUEne.png", ebiten.FilterDefault)
	sHeadD, _, _ := ebitenutil.NewImageFromFile("images/headDEne.png", ebiten.FilterDefault)
	sHeadL, _, _ := ebitenutil.NewImageFromFile("images/headLEne.png", ebiten.FilterDefault)
	sHeadR, _, _ := ebitenutil.NewImageFromFile("images/headREne.png", ebiten.FilterDefault)
	bodyE, _, _ := ebitenutil.NewImageFromFile("images/bodyEne.png", ebiten.FilterDefault)
	e.sHeadU = *sHeadU
	e.sHeadD = *sHeadD
	e.sHeadL = *sHeadL
	e.sHeadR = *sHeadR
	e.bodyE = *bodyE

	return &e
}

func (enemy *EnemySnake) ChannelPipe() error {
	for {
		dotTime := <-enemy.channelMovements
		err := enemy.Direction(dotTime)
		if err != nil {
			return err
		}
	}
}

// Direction updates the direction of the enemy randomly and verifies the boundaries of the map
func (enemy *EnemySnake) Direction(dotTime int) error {
	if dotTime == 1 {
		random := rand.New(enemy.seed)
		action := random.Intn(4)
		changingDirection := random.Intn(3)
		posX, posY := enemy.GetHeadPos()
		if changingDirection == 0 {
			switch action {
			case 0:
				if posX < 0 && enemy.lastDir != "left" {
					enemy.lastDir = "right"
				} else {
					enemy.lastDir = "left"
				}
			case 1:
				if posY < 40 && enemy.lastDir != "up" {
					enemy.lastDir = "down"
				} else {
					enemy.lastDir = "up"
				}
			case 2:
				if posY > 680 && enemy.lastDir != "down" {
					enemy.lastDir = "up"
				} else {
					enemy.lastDir = "down"
				}
			case 3:
				if posX > 580 && enemy.lastDir != "right" {
					enemy.lastDir = "left"
				} else {
					enemy.lastDir = "right"
				}
			}
		}
		// Bounds for the collisions
		if posX >= 580 {
			enemy.lastDir = "left"
		}
		if posX <= 0 {
			enemy.lastDir = "right"
		}
		if posY >= 680 {
			enemy.lastDir = "up"
		}
		if posY <= 40 {
			enemy.lastDir = "down"
		}
	}

	if dotTime == 1 { // Checks collision with enemy
		xPos, yPos := enemy.game.snake.getHeadPos()
		if enemy.CollSnake(xPos, yPos) {
			enemy.game.snake.game.crashed = true
			enemy.game.gameOver()
		}

		xPosE, yPosE := enemy.GetHeadPos()
		if enemy.game.crashCount > 0 && enemy.CollEnemy(xPosE, yPosE) {
			enemy.game.crashCount--
			if enemy.game.crashCount == 0{
				enemy.game.gameOver()
			}
		}

	}
	return nil
}

// Draw Draws the enemy snake
func (enemy *EnemySnake) Draw(screen *ebiten.Image, dotTime int) error {
	if enemy.game.alive {
		enemy.UpdatePos(dotTime)
	}
	enemyDO := &ebiten.DrawImageOptions{}
	xPos, yPos := enemy.GetHeadPos()
	enemyDO.GeoM.Translate(xPos, yPos)

	if enemy.lastDir == "up" {
		screen.DrawImage(&enemy.sHeadU, enemyDO)
	} else if enemy.lastDir == "down" {
		screen.DrawImage(&enemy.sHeadD, enemyDO)
	} else if enemy.lastDir == "right" {
		screen.DrawImage(&enemy.sHeadR, enemyDO)
	} else if enemy.lastDir == "left" {
		screen.DrawImage(&enemy.sHeadL, enemyDO)
	}

	for i := 0; i < enemy.nBodyP; i++ {
		partDO := &ebiten.DrawImageOptions{}
		xPos, yPos := enemy.GetBody(i)
		partDO.GeoM.Translate(xPos, yPos)
		screen.DrawImage(&enemy.bodyE, partDO)
	}

	return nil
}

// UpdatePos Updates head position and its score
func (enemy *EnemySnake) UpdatePos(dotTime int) {
	if dotTime == 1 {
		if enemy.pointsWaiting > 0 {
			enemy.nBodyP++
			enemy.pointsWaiting--
		}
		switch enemy.lastDir {
		case "up":
			enemy.TranslateHeadPos(0, -20)
		case "down":
			enemy.TranslateHeadPos(0, +20)
		case "right":
			enemy.TranslateHeadPos(20, 0)
		case "left":
			enemy.TranslateHeadPos(-20, 0)
		}

	}
}

// CollSnake Evaluating if there was a collision
func (enemy *EnemySnake) CollSnake(xPos, yPos float64) bool {
	for i := 0; i < len(enemy.partsOfBody); i++ {
		if xPos == enemy.partsOfBody[i][0] && yPos == enemy.partsOfBody[i][1] {
			return true
		}
	}
	return false
}

func (enemy *EnemySnake) CollEnemy(xPos, yPos float64) bool {
	for i := 0; i < len(enemy.game.snake.parts); i++ {
		if xPos == enemy.game.snake.parts[i].X && yPos == enemy.game.snake.parts[i].Y {
			return true
		}
	}
	return false
}


// GetHeadPos Head pos is returned
func (enemy *EnemySnake) GetHeadPos() (float64, float64) {
	return enemy.partsOfBody[0][0], enemy.partsOfBody[0][1]
}

// GetBody Last body pos is returned
func (enemy *EnemySnake) GetBody(pos int) (float64, float64) {
	return enemy.partsOfBody[pos+1][0], enemy.partsOfBody[pos+1][1]
}

// AddPoint controls enemy's score
func (enemy *EnemySnake) AddPoint() {
	enemy.score++
	enemy.pointsWaiting++
}

// AddParts add a body part
func (enemy *EnemySnake) AddParts(newX, newY float64) {
	enemy.partsOfBody = append([][]float64{{newX, newY}}, enemy.partsOfBody...)
	enemy.partsOfBody = enemy.partsOfBody[:enemy.nBodyP+1]
}

// TranslateHeadPos Changes head position
func (enemy *EnemySnake) TranslateHeadPos(newXPos, newYPos float64) {
	newX := enemy.partsOfBody[0][0] + newXPos
	newY := enemy.partsOfBody[0][1] + newYPos
	enemy.AddParts(newX, newY)
}
