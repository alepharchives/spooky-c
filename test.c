#include <sys/fcntl.h>
#include <stdio.h>
#include "spooky-c.h"
#include "map.h"

#define use_value(x) asm volatile("" :: "r" (x) : "memory")

int main(int ac, char **av)
{
	while (*++av) {
		uint64_t h1, h2;
		struct spooky_state state;
		size_t size;
		char *map = mapfile(*av, O_RDONLY, &size);
		if (!map) {
			perror(*av);
			continue;
		}
		
		int i;
		for (i = 0; i < size; size += 64)
			use_value(((volatile char *)map)[i]);

		spooky_init(&state, 0x123456789abcdef, 0xfedcba987654321);
		spooky_update(&state, map, size);
		spooky_final(&state, &h1, &h2);

		printf("%s: %016llx%016llx\n", *av, 
		       (unsigned long long)h1, 
		       (unsigned long long)h2);

		unmap_file(map, size);
	}
	return 0;
}

