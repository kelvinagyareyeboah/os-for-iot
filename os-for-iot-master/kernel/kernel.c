
// Forward declaration of the real kernel_main
void kernel_main(void);

void _start(void)
{
    kernel_main();
    while (1) {}
}

python3 -m venv venv
source venv/bin/activate
pip install customtkinter

pkill qemu-system-arm
