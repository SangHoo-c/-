package scrapper

import (
	"encoding/csv"
	"fmt"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"

	"github.com/PuerkitoBio/goquery"
)

// create object
type extractedJob struct {
	id       string
	title    string
	location string
	summary  string
}

// Scrape Indeed by a term.
func Scrape(term string) {
	var baseURL = "https://kr.indeed.com/jobs?q=" + term + "&l&vjk=1015284880e2ff62"
	var jobs []extractedJob
	c := make(chan []extractedJob)

	totalPages := getPages(baseURL)
	for i := 0; i < totalPages; i++ {
		go getPage(baseURL, i, c) // main 아래에 총 totalPage 만큼 반복되는 getPage 함수 동시에 적용
	}

	for i := 0; i < totalPages; i++ {
		job := <-c
		jobs = append(jobs, job...)
	}

	// getPage(baseURL)
	// fmt.Println(jobs)
	writeJobs(jobs)
	fmt.Println("Done, extracted", len(jobs))
}

// pageNum = 0 이면, 그냥 l  나머진 start=10 * pageNum
func getPage(url string, pageNum int, mainC chan []extractedJob) {
	// 1. url parsing
	// 2. 가운데 변경
	// 3. connection 체크

	newURL := url
	// 1.
	slice := strings.Split(url, "&")

	// 2.
	if pageNum != 0 {
		newURL = ""
		newURL += slice[0]
		newURL += "&start=" + strconv.Itoa(10*pageNum) + "&"
		newURL += slice[2]
	}
	fmt.Println("requesting URL : ", newURL)
	// 3.
	resp, err := http.Get(newURL)
	checkErr(err)
	checkCode(resp)
	defer resp.Body.Close()

	doc, err := goquery.NewDocumentFromReader(resp.Body)
	checkErr(err)

	searchCards := doc.Find(".resultWithShelf")
	var jobs []extractedJob
	c := make(chan extractedJob)
	// jobs := []extractedJob

	searchCards.Each(func(i int, card *goquery.Selection) {
		go extractJob(card, c)
		// jobs = append(jobs, job)
	})

	for i := 0; i < searchCards.Length(); i++ {
		job := <-c
		jobs = append(jobs, job)
	}

	mainC <- jobs

}

func extractJob(card *goquery.Selection, c chan<- extractedJob) {
	id, _ := card.Attr("data-jk")
	title := CleanString(card.Find(".jobTitle>span").Text()) // jobTitle 클래스 안에 있는 span 의 Text
	location := CleanString(card.Find(".companyLocation").Text())
	summary := CleanString(card.Find(".job-snippet").Text())
	c <- extractedJob{
		id:       id,
		title:    title,
		location: location,
		summary:  summary,
	}
}

// TrimSpace : 양쪽에 공백을 지운다.
// string.Fields : text 으로 이루어진 배열을 반환
// join : 합친다.
func CleanString(str string) string {
	return strings.Join(strings.Fields(strings.TrimSpace(str)), " ")
}

func getPages(baseURL string) int {
	pages := 0
	resp, err := http.Get(baseURL)
	checkErr(err)
	checkCode(resp)
	defer resp.Body.Close()

	doc, err := goquery.NewDocumentFromReader(resp.Body)
	checkErr(err)

	doc.Find(".pagination").Each(func(i int, s *goquery.Selection) {
		pages = s.Find("a").Length()
	})

	return pages
}

func checkErr(err error) {
	if err != nil {
		log.Fatalln(err)
	}
}

func checkCode(resp *http.Response) {
	if resp.StatusCode != 200 {
		log.Fatalln("Request failed with status code : ", resp.StatusCode)
	}
}

func writeJobs(jobs []extractedJob) {
	file, err := os.Create("jobs.csv")
	checkErr(err)
	w := csv.NewWriter(file)
	defer w.Flush() // Flush writes any buffered data to the underlying io.Writer.

	headers := []string{"ID", "Title", "Location", "Summary"}

	wErr := w.Write(headers)
	checkErr(wErr)

	for _, job := range jobs {
		jobSlice := []string{job.id, job.title, job.location, job.summary}
		jwErr := w.Write(jobSlice)
		checkErr(jwErr)
	}
}
