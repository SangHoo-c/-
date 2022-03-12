# [1] types

```tsx
const name = "hoo",
age = 26, gender="male";

// gender? = option, 인자로 전달해도 되고 안해도 된다. 
const hello = (name, age, gender?) => {
    console.log(`Hello ${name}, your ar ${age}, ${gender}`);
};
hello(name, age)

export {};
```

- export {};
    - 선언한 (함수,상수,클래스) 를 export 해서 쓸 수 있게 하는 것
- 인자 option 설정

<br/>

```tsx
const hello = (name: string, age: number, gender: string): string => {
    return `Hello ${name}, your ar ${age}, ${gender}`;
};

console.log(hello("HOO", 23, "male"));

export {hello};
```

- 인자 type 명시 가능
    
    > Argument of type 'string' is not assignable to parameter of type 'number'.ts(2345)
    > 
- return type 설정 가능
- 사람 정보를 변수로 넣지 말고, interface 를 사용하면 어떨까?

<br/>

```tsx
interface Human {
    name: string;
    age: number;
    gender: string;
}

const person = {
    name: "JURI",
    age: 22,
    gender: "female"
}

const hello = (person: Human): string => {
    return `Hello ${person.name}, your ar ${person.age}, ${person.gender}`;
};

console.log(hello(person))

export {hello};
```

- interface 의 attribute 에 접근 가능
