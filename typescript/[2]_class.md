# [2]

interface & class

```tsx
// 코드를 조금 더 control 할 수 있는 힘! 
class Human {
    public name: string; // permission & property's type 
    public age: number;
    public gender: string;
    constructor(name:string, age:number, gender:string){
        this.name = name;
        this.age = age;
        this.gender = gender;
    }
}

// js 로 컴파일시에 보이지 않는다. 
interface Human {
    name: string;
    age: number;
    gender: string;
}
```

- constructor : object 가 생성될 때 마다 실행되는 method

[interface vs class](https://stackoverflow.com/questions/51716808/when-use-a-interface-or-class-in-typescript)  

- **class** may have initialized properties and methods to help create objects
- **interface** essentially defines the properties and type an object can have.
- class 사용시 privacy 적용 가능 (public / private)

<br/>

```tsx
class Block {
    ...
}

const genesisBlock: Block = new Block(0, "234234234", "", "Hello", 123456);

let blockchain: [Block] = [genesisBlock];
blockchain.append("stuff"); 
```

- Property 'append' does not exist on type '[Block]'.ts
    - “stuff” 이 Block type 이 아니다.
- blockchain 의 type 을 미리 선언해준다.

<br/>

crypto-js

```tsx
import * as CryptoJS from 'crypto-js';

class Block {
		...
    static calculateBlockHash = (
        index: number, 
        previousHash: string, 
        timestamp: number, 
        data: string
    ): string => 
        CryptoJS.SHA256(index + previousHash + timestamp + data).toString();
    ...
}

const genesisBlock: Block = new Block(0, "234234234", "", "Hello", 123456);
let blockchain: Block[] = [genesisBlock]; // first input 

const getBlockChain = (): Block[] => blockchain;
const getLatestBlock = (): Block => blockchain[blockchain.length - 1];
const getNewTimeStamp = (): number => Math.round(new Date().getTime() / 1000);

export {}
```

- static method
    - class 안에서 언제든지 사용가능한 method
    - class 생성했을때만 사용가능
- **static vs non-static**
    - 왜 static method 사용할까?
    - static class method is that `belongs to the class itself`, not the instance of a class.
    - Static methods are a great concept to share functionality from a class `without instantiating the class.`
    - At the memory level, a portion of the memory will be created for the static fields, 
    which will be `shared across all objects in the class.` [[참조링크]](https://stackoverflow.com/questions/52481219/any-reason-to-use-static-private-static-methods-in-typescript)
