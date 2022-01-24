# Basic

**import, package**

```go
// main 은 entry point
// 컴파일러가 여기서부터 시작!
// compile 되고 싶지 않다면 main 만들지 않아도 된다.

package main

import (
	"fmt"
	// "github.com/SangHoo-c/learngo/something"
)

func main() {
	fmt.Println("Hello World!! ")
	// something.SayHello() // upper case. 다른 package 에서 export 된 function
}
```

**constant, variables**

```go
package main

import "fmt"

name := "HOO" // (x)

func main() {
	const name string = "SH" // constant, 변경 불가 
	name = "LYNN"  // (x)

	var name string = "SH" // 사용자 지정 Type
	name := "SH"           // Go 가 type 을 추론해준다.

	name = "LYNN"
	fmt.Println(name)
}
```

**function # 1.**

how it works! 

```go
package main

import "fmt"

func multiply(a, b int) int {
	return a * b
}

func main() {
	fmt.Println(multiply(2, 3))
}
```

multiple input & output 

```go
package main

import (
	"fmt"
	"strings"
)

func lenAndUpper(name string) (int, string) {
	return len(name), strings.ToUpper(name)
}

func main() {
	totalLength, upperName := lenAndUpper("sanghoo")
	fmt.Println(totalLength, upperName)
}
```

multiple arguments 

```go
package main

import (
	"fmt"
)

func repeatMe(words ...string) {
	fmt.Println(words)
}

func main() {
	repeatMe("nico", "lynn", "dal", "hoo")
}
```

**function # 2.**

naked return & ***defer (짱 좋음!)***  

```go
package main

import (
	"fmt"
	"strings"
)

func lenAndUpper(name string) (lenght int, uppercase string) {
	defer fmt.Println("I'm done") // (defer) after function finished => 실행
	lenght = len(name)            // length is not new. 앞에서 미리 생성되어 있음
	uppercase = strings.ToUpper(name)
	return // naked return.
}

func main() {
	totalLength, upperCase := lenAndUpper("hoo") // 초기화 할땐, := 필요.
	fmt.Println(totalLength, upperCase)
}

// output 
// I'm done
// 3 HOO
```

**for # 1.** 

```go
func superAdd(numbers ...int) int {

	// # 1. 전통적인 방법 
	for i := 0; i < len(numbers); i++ {
		fmt.Println(i, numbers[i])
	}
	
	// # 2. 'range' 추천! index 도 함께 사용가능 
	for index, number := range numbers {
		fmt.Println(index, number)
	}
	return 1
}
```

**for # 2.**  `range` 를 사용하자!!!

```go
func superAdd(numbers ...int) int {
	total := 0
	for _, number := range numbers {
		total += number
	}
	return total
}

func main() {
	total := superAdd(1, 2, 3, 4, 5, 6)
	fmt.Println(total)
}
```

**if**

```go
func canIDrink(age int) bool {

	// 다시 사용하지 않을 변수 (koreanAge) 를 사용할 경우,
	// 읽는 사람에게 hint 를 줄 수 있다.
	if koreanAge := age + 2; koreanAge < 18 {
		return false
	}
	return true
}

func main() {
	fmt.Println(canIDrink(16))
}
```

**switch**

```go
func canIDrink(age int) bool {
	switch koreanAge := age + 2; {
	case koreanAge < 18:
		return false
	case koreanAge == 18:
		return true
	}
	return false
}
```

**pointer** 

- how to look addresses
- how to connect things

> reference? 언제 쓸까? ⇒ 무거운 데이터 구조를 다루고 있을때! 
같은 object 가 필요하다~!? 
copy 하지 않고 `in-memory` 에서 같이 쓸 수 있다!
> 

```go
func main() {
	a := 2
	b := &a // address of a
	*b = 17 // b is a pointer of 'a'

	fmt.Println(&a, b)
	fmt.Println(a, *b)
}
```

**slice** (크기가 정해져 있지 않는 array)

```go
func main() {
	names := []string{"nico", "lynn", "dal"}
	names = append(names, "flynn")  // 함수형(input & output) 구조 

	fmt.Println(names)
}
```
