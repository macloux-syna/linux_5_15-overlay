/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2021 - 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ISP_DMAHEAP_H__
#define __ISP_DMAHEAP_H__

enum memory_type_t {
	SHM_NONSECURE_CONTIG  = 0,
	SHM_NONSECURE_NON_CONTIG,
	SHM_MAX_MEMORY_TYPE,
};

static inline void *
isp_dma_heap_plane_vaddr(struct vb2_buffer *vb, unsigned int plane_no)
{
	void *addr = vb2_plane_vaddr(vb, plane_no);

	return addr;
}

static inline void *
isp_dma_heap_plane_cookie(struct vb2_buffer *vb, unsigned int plane_no)
{
	void *addr = vb2_plane_cookie(vb, plane_no);

	return addr;
}

int isp_dma_heap_dev_alloc(struct device **pdev);
void isp_dma_heap_dev_release(void);
void *isp_dma_heap_get_phyaddr(void *handle);
void *isp_dma_heap_get_pagetbl_phyaddr(void *handle);

extern const struct vb2_mem_ops isp_dma_contig_memops;

#endif
