package mydict

import "errors"

// Dictionary
type Dictionary map[string]string

var KeyNotFoundError = errors.New("Key Not Found Errors.")
var KeyExistsError = errors.New("Key Already Exists Error")

func (d Dictionary) Search(key string) (string, error) {
	value, exists := d[key]
	if exists {
		return value, nil
	}
	return "", KeyNotFoundError
}

// Add a word to a dictionary
func (d Dictionary) Add(key, value string) error {
	_, err := d.Search(key)
	switch err {
	case KeyNotFoundError:
		d[key] = value
	case nil:
		return KeyExistsError
	}
	return nil
}

// Update a value.
// 1. 해당 key 에 value 가 존재해야해
// 		key 없는 경우 : KeyNotFoundError
// 2. value 를 바꿔줘야해.
func (d Dictionary) Update(key, value string) error {
	_, err := d.Search(key)
	if err != nil {
		return KeyNotFoundError
	} else {
		d[key] = value
	}
	return nil
}

// Delete a value.
func (d Dictionary) Delete(key string) {
	delete(d, key)
}
