import copy

a = [17, 1, 3, 4, 6, 15]


def binary_search(arr, target):
    data = copy.deepcopy(arr)
    data.sort()
    left = 0
    right = len(data) - 1
    while left <= right:
        mid = (left + right) // 2
        if target == data[mid]:
            print('find!')
            print(mid)
            break
        elif target < data[mid]:
            right = mid - 1
        elif target > data[mid]:
            left = mid + 1
        else:
            pass


def binary_search_rec(data, target, left, right, cnt):
    if cnt == 0:
        data = copy.deepcopy(data)
        data.sort()
    if left > right:
        return
    mid = (left + right) // 2
    if data[mid] == target:
        print("find !!")
        return mid
    elif target < data[mid]:
        right = mid - 1
    elif target > data[mid]:
        left = mid + 1
    else:
        pass

    return binary_search_rec(data, target, left, right, cnt + 1)


binary_search(a, 3)
binary_search_rec(a, 3, 0, len(a) - 1, 0)
