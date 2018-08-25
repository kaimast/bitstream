# bitstream: Quickly serialize data in C++17

If you feel like Google protocol buffers are too complex for your project, bitstreams are here for the resuce!

## Usage
### Reading and Writing
Bitstream are used like any other stream in C++. Just overload to the << and >> operators to (de-)serialize data. It comes with overloads for all basic STL cotainers, such as list, set, or vector, and PODs need no specialized operator at all.

```C++
const int in_value = 42;
int out_value;

bitstream bs;

// Write the input to the string
bs << in_value;

// Go back to the beginning of the stream
bs.move_to(0);

// Write the data to the output
bs >> out_value;

EXPECT_TRUE(in_value, out_value);
```

### Views
You can easily assign a bitstream to operate on a subset of another bitstream. Use the assign() operator for that.

```C++
const std::string foo = "bar";

bitstream bs;
bs << foo;

// create a read only view of the data
bistream view;
view.assign(bs.data(), bs.size(), true); 
```

## Building
There is no need to compile this project, it is headers-only. Just copy the bistream.h to your include directory or project.

Alternatively, you can use meson and ninja to install the header as well as build tests. You need to have google-test installed for this.

```bash
meson build
cd build
sudo ninja install
```

## Related Projects
bistreams are used by the following projects

* [Yet another event loop](https://github.com/kaimast/yael)
* [libdocument](https://github.com/kaimast/libdocument)
