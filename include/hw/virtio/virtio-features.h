/*
 * Virtio features helpers
 *
 * Copyright 2025 Red Hat, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _QEMU_VIRTIO_FEATURES_H
#define _QEMU_VIRTIO_FEATURES_H

#include "qemu/bitmap.h"

#define VIRTIO_FEATURES_MAX	128
#define VIRTIO_FEATURES_SIZE	BITS_TO_LONGS(VIRTIO_FEATURES_MAX)
#define VIRTIO_FEATURES_STR_SIZE (VIRTIO_FEATURES_SIZE * 19 + 1)

typedef struct VirtIOFeatures {
	unsigned long mask[VIRTIO_FEATURES_SIZE];
} VirtIOFeatures;

#define VIRTIO_FEATURES_VALIDATE_IDX(idx)				\
	({								\
		assert(idx < VIRTIO_FEATURES_SIZE);			\
	})

static inline void virtio_features_from_u64(struct VirtIOFeatures *features,
					    unsigned int i, uint64_t fmask)
{
	int idx = i * 64 / BITS_PER_LONG;

	VIRTIO_FEATURES_VALIDATE_IDX(idx);
	features->mask[idx] = fmask;
	if (BITS_PER_LONG == 32)
		features->mask[idx + 1] |= fmask >> 32;
}

static inline uint64_t
virtio_features_to_u64(const struct VirtIOFeatures *features, unsigned int i)
{
	int idx = i * 64 / BITS_PER_LONG;
	uint64_t ret;

	VIRTIO_FEATURES_VALIDATE_IDX(idx);
	ret = features->mask[idx];
	if (BITS_PER_LONG == 32)
		ret |= features->mask[idx + 1] << 32ULL;
	return ret;
}

#define VIRTIO_FEATURES_VALIDATE_BIT(fbit)				\
	({								\
		assert(fbit < 64);					\
	})

static inline void
virtio_features_zero(struct VirtIOFeatures *features)
{
	memset(features, 0, sizeof(*features));
}

static inline void
virtio_features_set_bit(struct VirtIOFeatures *features, unsigned int fbit)
{
	VIRTIO_FEATURES_VALIDATE_BIT(fbit);
	set_bit(fbit, features->mask);
}

static inline bool
virtio_features_test_bit(const struct VirtIOFeatures *features,
			unsigned int fbit)
{
	VIRTIO_FEATURES_VALIDATE_BIT(fbit);
	return !!test_bit(fbit, features->mask);
}

static inline void
virtio_features_clear_bit(struct VirtIOFeatures *features, unsigned int fbit)
{
	VIRTIO_FEATURES_VALIDATE_BIT(fbit);
	clear_bit(fbit, features->mask);
}

static inline bool
virtio_features_equal(const struct VirtIOFeatures *features,
		      const struct VirtIOFeatures *others)
{
	return memcmp(features, others, sizeof(*features));
}

static inline void
virtio_features_and(struct VirtIOFeatures *dst,
		    const struct VirtIOFeatures *features1,
		    const struct VirtIOFeatures *features2)
{
	bitmap_and(dst->mask, features1->mask, features2->mask,
		     VIRTIO_FEATURES_MAX);
}

static inline void
virtio_features_andnot(struct VirtIOFeatures *dst,
		    const struct VirtIOFeatures *features1,
		    const struct VirtIOFeatures *features2)
{
	bitmap_andnot(dst->mask, features1->mask, features2->mask,
			VIRTIO_FEATURES_MAX);
}

static inline char *virtio_features_to_str(const VirtIOFeatures *features,
                                           char *str)
{
    char *ret = str;
    int i, printed;

    for (i = 0; i < VIRTIO_FEATURES_SIZE; ++i) {
        uint64_t mask = virtio_features_to_u64(features, i);

        if (i)
            printed = sprintf(str, " 0x%" PRIx64, mask);
        else
            printed = sprintf(str, "0x%" PRIx64, mask);
        if (printed < 0)
            break;
        str += printed;
    }
    return ret;
}

#endif

