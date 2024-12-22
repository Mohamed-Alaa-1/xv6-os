static unsigned int seed = 123456;

unsigned int random() {
  seed = seed * 1103515245 + 12345;
  return (seed / 65536) % 32768;
}
