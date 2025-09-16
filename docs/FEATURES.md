# Language Features

### Variable Assignment 
Variables are dynamically typed.
```c
a = 123; // Returns 123

b = 32.6; // Returns 32.6

c = "Hello World" // Returns Hello World
```

### Basic Operators
Support for all standard arithmetic operators, the second operand is automatically casted when required.
```c
1 + 2; // Returns 3

3.2 - 1; // Returns 2.2
```
The \+ operator is overloaded for string concatenation.
```c
"Hello " + "World"; // Returns Hello World
```

### If-Else Statements
```c
if a <= b do {
    c = 1;
} else { // Optional branch
    c = 2;
}

// Curly braces are not required
c = if a <= b do 1 else 2;
```

### While Loops
```c
while a <= b do {
    // Body here
    a = a + 1;
}
```

### For Loops
For loops have three parameters, an initialiser, a condition, and an increment.
```c
for int i = 0; i <= 10; i = i + 1; {
    >> i; // Prints numbers 0 to 10 inclusive
}
```

### Functions
```c
// Without parameters
def a => 1 + 2;

// Call with the identifier
a; // Returns 3

// Or with parameters
def foo(x, y) do {
    x = x + 1;
    y = y - 1;
    x * y; // The final value is automatically returned
}

foo(5, 3); // Returns 12
```

### Print to Console
```c
>> "Hello World" // Prints Hello World
```

### Lists
```c
a = [1,2,3] // List initialisation
b = [1, 0.1, "quokka"] // You can mix types

b[2] //"quokka"

c = a + b; // [1,2,3,1,0.1,"quokka"]
```

### Classes and Objects
```c
class Car(colour) {
    speed = 10; // Any initialisation goes here

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
