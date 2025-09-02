# Language Features

### Basic Arithmetic
```c
1 + 2; // Returns 3
```

### Variable Assignment 
```c
a = 123;

a; // Returns 123
```

### If-Else Statements
```c
if a <= b do {
    c = 1;
} else { // Optional branch
    c = 2;
}

// Or in this case it can be simpler
c = if a <= b do 1 else 2;
```

### While Loops
```c
while a <= b do {
    // Body here
    a = a + 1;
}
```

### Functions
```c
// Without parameters
def a => 1 + 2;

// Call with the identifier
a; // Returns 3

// Or with parameters
def foo(x, y) => {
    x = x + 1;
    y = y - 1;
    x * y; // The final value is automatically returned
}

foo(5, 3); // Returns 12
```

### Basic Print Statements
```c
>> "Hello World" // Outputs Hello World to the console
```

### Class
```c
class Car(colour) {
    speed = 10; 

    def print_colour => {
        >> colour;
    }

    def set_colour(col) => {
        // Use the set keyword to assign the variable to the objet
        set colour = col; 
    }
}

car = Car("red");

car.print_colour; // red

car.set_colour("blue");

car.print_colour; // blue

```
