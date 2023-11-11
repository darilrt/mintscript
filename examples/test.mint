
mut i: int = 0

while i < 10 {
    i = i + 1
    
    if i == 5 {
        print("continuing")
        continue
    }
    
    print(i)

    if i == 7 {
        print("breaking")
        break
    }
}

print("done ", i)