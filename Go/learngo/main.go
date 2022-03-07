package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/SangHoo-c/learngo/scrapper"
	"github.com/labstack/echo"
)

func handleHome(c echo.Context) error {
	return c.File("home.html")
}

const fileName string = "jobs.csv"

func handleScrape(c echo.Context) error {
	defer os.Remove(fileName)
	term := strings.ToLower(scrapper.CleanString(c.FormValue("term")))
	fmt.Println(term)
	scrapper.Scrape(term)

	return c.Attachment(fileName, fileName)
}

func main() {
	e := echo.New()
	e.GET("/", handleHome)
	e.POST("/scrape", handleScrape)

	e.Logger.Fatal(e.Start(":1323"))
}
