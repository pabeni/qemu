/*
 * Virtio features helpers
 *
 * Copyright 2025 Red Hat, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _QEMU_VIRTIO_FEATURES_H
#define _QEMU_VIRTIO_FEATURES_H

#ifdef CONFIG_INT128
#define VIRTIO_BIT(b)			((__int128_t)1 << b)
#define VIRTIO_FEATURES_WORDS		4
#define VIRTIO_FEATURES_FMT		"%016"PRIx64"%016"PRIx64
#define VIRTIO_FEATURES_PRN_ARG(f)	(uint64_t)((f) >> 64), (uint64_t)(f)

typedef __uint128_t		virtio_features_t;

#if HOST_BIG_ENDIAN
#define DECLARE_FEATURES(name)			\
	union {					\
		struct {			\
			uint64_t name##_hi;	\
			uint64_t name;		\
		};				\
		__uint128_t	name##_ex;	\
	}
#else
#define DECLARE_FEATURES(name)			\
	union {					\
		struct {			\
			uint64_t name;		\
			uint64_t name##_hi;	\
		};				\
		__uint128_t	name##_ex;	\
	}
#endif

static inline void virtio_add_feature_ex(__uint128_t *features, unsigned int fbit)
{
    assert(fbit < 128);
    *features |= VIRTIO_BIT(fbit);
}

static inline void virtio_clear_feature_ex(__uint128_t *features, unsigned int fbit)
{
    assert(fbit < 128);
    *features &= ~VIRTIO_BIT(fbit);
}

static inline bool virtio_has_feature_ex(__uint128_t features, unsigned int fbit)
{
    assert(fbit < 128);
    return !!(features & VIRTIO_BIT(fbit));
}

#else /* !CONFIG_INT128 */

#define VIRTIO_BIT(b)			(1ULL << b)
#define VIRTIO_FEATURES_WORDS		2
#define VIRTIO_FEATURES_FMT		"%"PRIx64
#define VIRTIO_FEATURES_PRN_ARG(f)	f

typedef uint64_t		virtio_features_t;

/*
 * Without 128 bits support, 'features_ex' is just an alias for the 64 bits
 * variable. This help avoiding conditionals in the core virtio code
 * manipulation the features
 */
#define DECLARE_FEATURES(name)			\
	union {					\
		uint64_t name;			\
		uint64_t name##_ex;		\
	}

#define virtio_clear_feature_ex		virtio_clear_feature
#define virtio_add_feature_ex		virtio_add_feature
#define virtio_has_feature_ex		virtio_has_feature

#endif

#endif

