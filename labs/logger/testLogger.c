int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);

int main() {
    // INFO
    infof("This is a regular message");

    // WARNING
    warnf("Okay now is a warning");

    // ERROR
    errorf("Holy cow we got an error");

    // PANIC
    panicf("EVERYBODY PANIC!!1");

    return 0;
}
