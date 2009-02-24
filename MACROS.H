#define KEY_DOWN(i) ((GetAsyncKeyState(i) & 0x8000) ? 1 : 0)
#define KEY_UP(i) ((GetAsyncKeyState(i) & 0x8000) ? 0 : 1)

#define SIGN(x) (x)/abs(x)
#define MAX(x,y) (((x)>(y))?(x):(y))
#define MIN(x,y) (((x)<(y))?(x):(y))