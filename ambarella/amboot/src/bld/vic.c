/**
 * bld/vic.c
 *
 * Vector interrupt controller related utilities.
 *
 * History:
 *    2005/07/26 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <amboot.h>
#include <bldfunc.h>
#include <ambhw/vic.h>

/* ==========================================================================*/
void enable_interrupts(void)
{
	unsigned long tmp;

	__asm__ __volatile__ (
		"mrs %0, cpsr\n"
		"bic %0, %0, #0x80\n"
		"msr cpsr_c, %0"
		: "=r" (tmp)
		:
		: "memory");
}

void disable_interrupts(void)
{
	unsigned long old;
	unsigned long tmp;

	__asm__ __volatile__ (
		"mrs %0, cpsr\n"
		"orr %1, %0, #0xc0\n"
		"msr cpsr_c, %1"
		: "=r" (old), "=r" (tmp)
		:
		: "memory");
}

/* ==========================================================================*/
void vic_init(void)
{
	disable_interrupts();

	/* Set VIC sense and event type for each entry
	 * note: we initialize udc vbus irq type here */
	writel(VIC_REG(VIC_SENSE_OFFSET), 0x00000001);
	writel(VIC_REG(VIC_BOTHEDGE_OFFSET), 0x00000000);
	writel(VIC_REG(VIC_EVENT_OFFSET), 0x00000001);
#if (VIC_INSTANCES >= 2)
	writel(VIC2_REG(VIC_SENSE_OFFSET), 0x00000000);
	writel(VIC2_REG(VIC_BOTHEDGE_OFFSET), 0x00000000);
	writel(VIC2_REG(VIC_EVENT_OFFSET), 0x00000000);
#endif
#if (VIC_INSTANCES >= 3)
	writel(VIC3_REG(VIC_SENSE_OFFSET), 0x00000000);
	writel(VIC3_REG(VIC_BOTHEDGE_OFFSET), 0x00000000);
	writel(VIC3_REG(VIC_EVENT_OFFSET), 0x00000000);
#endif
#if (VIC_INSTANCES >= 4)
	writel(VIC4_REG(VIC_SENSE_OFFSET), 0x00000000);
	writel(VIC4_REG(VIC_BOTHEDGE_OFFSET), 0x00000000);
	writel(VIC4_REG(VIC_EVENT_OFFSET), 0x00000000);
#endif
	/* Disable all IRQ */
	writel(VIC_REG(VIC_INT_SEL_OFFSET), 0x00000000);
	writel(VIC_REG(VIC_INTEN_OFFSET), 0x00000000);
	writel(VIC_REG(VIC_INTEN_CLR_OFFSET), 0xffffffff);
	writel(VIC_REG(VIC_EDGE_CLR_OFFSET), 0xffffffff);
	writel(VIC_REG(VIC_INT_PTR0_OFFSET), 0xffffffff);
	writel(VIC_REG(VIC_INT_PTR1_OFFSET), 0x00000000);
#if (VIC_INSTANCES >= 2)
	writel(VIC2_REG(VIC_INT_SEL_OFFSET), 0x00000000);
	writel(VIC2_REG(VIC_INTEN_OFFSET), 0x00000000);
	writel(VIC2_REG(VIC_INTEN_CLR_OFFSET), 0xffffffff);
	writel(VIC2_REG(VIC_EDGE_CLR_OFFSET), 0xffffffff);
	writel(VIC2_REG(VIC_INT_PTR0_OFFSET), 0xffffffff);
	writel(VIC2_REG(VIC_INT_PTR1_OFFSET), 0x00000000);
#endif
#if (VIC_INSTANCES >= 3)
	writel(VIC3_REG(VIC_INT_SEL_OFFSET), 0x00000000);
	writel(VIC3_REG(VIC_INTEN_OFFSET), 0x00000000);
	writel(VIC3_REG(VIC_INTEN_CLR_OFFSET), 0xffffffff);
	writel(VIC3_REG(VIC_EDGE_CLR_OFFSET), 0xffffffff);
	writel(VIC3_REG(VIC_INT_PTR0_OFFSET), 0xffffffff);
	writel(VIC3_REG(VIC_INT_PTR1_OFFSET), 0x00000000);
#endif
#if (VIC_INSTANCES >= 4)
	writel(VIC4_REG(VIC_INT_SEL_OFFSET), 0x00000000);
	writel(VIC4_REG(VIC_INTEN_OFFSET), 0x00000000);
	writel(VIC4_REG(VIC_INTEN_CLR_OFFSET), 0xffffffff);
	writel(VIC4_REG(VIC_EDGE_CLR_OFFSET), 0xffffffff);
	writel(VIC4_REG(VIC_INT_PTR0_OFFSET), 0xffffffff);
	writel(VIC4_REG(VIC_INT_PTR1_OFFSET), 0x00000000);
#endif

	enable_interrupts();
}

void vic_set_type(u32 line, u32 type)
{
	u32 mask = 0x0, sense = 0x0 , bothedges = 0x0, event = 0x0;
	u32 bit = 0x0;

	if (line < VIC_INT_VEC(NR_VIC_IRQ_SIZE)) {
		mask = ~(0x1 << line);
		bit = (0x1 << line);

		/* Line directly connected to VIC */
		sense = readl(VIC_REG(VIC_SENSE_OFFSET));
		bothedges = readl(VIC_REG(VIC_BOTHEDGE_OFFSET));
		event = readl(VIC_REG(VIC_EVENT_OFFSET));
#if (VIC_INSTANCES >= 2)
	} else if (line < VIC2_INT_VEC(NR_VIC_IRQ_SIZE)) {
		mask = ~(0x1 << (line - VIC2_INT_VEC(0)) );
		bit = (0x1 << (line - VIC2_INT_VEC(0)) );

		/* Line directly connected to VIC */
		sense = readl(VIC2_REG(VIC_SENSE_OFFSET));
		bothedges = readl(VIC2_REG(VIC_BOTHEDGE_OFFSET));
		event = readl(VIC2_REG(VIC_EVENT_OFFSET));
#endif
#if (VIC_INSTANCES >= 3)
	} else if (line < VIC3_INT_VEC(NR_VIC_IRQ_SIZE)) {
		mask = ~(0x1 << (line - VIC3_INT_VEC(0)) );
		bit = (0x1 << (line - VIC3_INT_VEC(0)) );

		/* Line directly connected to VIC */
		sense = readl(VIC3_REG(VIC_SENSE_OFFSET));
		bothedges = readl(VIC3_REG(VIC_BOTHEDGE_OFFSET));
		event = readl(VIC3_REG(VIC_EVENT_OFFSET));
#endif
#if (VIC_INSTANCES >= 4)
	} else if (line < VIC4_INT_VEC(NR_VIC_IRQ_SIZE)) {
		mask = ~(0x1 << (line - VIC4_INT_VEC(0)) );
		bit = (0x1 << (line - VIC4_INT_VEC(0)) );

		/* Line directly connected to VIC */
		sense = readl(VIC4_REG(VIC_SENSE_OFFSET));
		bothedges = readl(VIC4_REG(VIC_BOTHEDGE_OFFSET));
		event = readl(VIC4_REG(VIC_EVENT_OFFSET));
#endif
	}

	switch (type) {
	case VIRQ_RISING_EDGE:
		sense &= mask;
		bothedges &= mask;
		event |= bit;
		break;
	case VIRQ_FALLING_EDGE:
		sense &= mask;
		bothedges &= mask;
		event &= mask;
		break;
	case VIRQ_BOTH_EDGES:
		sense &= mask;
		bothedges |= bit;
		event &= mask;
		break;
	case VIRQ_LEVEL_LOW:
		sense |= bit;
		bothedges &= mask;
		event &= mask;
		break;
	case VIRQ_LEVEL_HIGH:
		sense |= bit;
		bothedges &= mask;
		event |= bit;
		break;
	}

	if (line < VIC_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC_REG(VIC_SENSE_OFFSET), sense);
		writel(VIC_REG(VIC_BOTHEDGE_OFFSET), bothedges);
		writel(VIC_REG(VIC_EVENT_OFFSET), event);
#if (VIC_INSTANCES >= 2)
	} else if (line < VIC2_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC2_REG(VIC_SENSE_OFFSET), sense);
		writel(VIC2_REG(VIC_BOTHEDGE_OFFSET), bothedges);
		writel(VIC2_REG(VIC_EVENT_OFFSET), event);
#endif
#if (VIC_INSTANCES >= 3)
	} else if (line < VIC3_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC3_REG(VIC_SENSE_OFFSET), sense);
		writel(VIC3_REG(VIC_BOTHEDGE_OFFSET), bothedges);
		writel(VIC3_REG(VIC_EVENT_OFFSET), event);
#endif
#if (VIC_INSTANCES >= 4)
	} else if (line < VIC4_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC4_REG(VIC_SENSE_OFFSET), sense);
		writel(VIC4_REG(VIC_BOTHEDGE_OFFSET), bothedges);
		writel(VIC4_REG(VIC_EVENT_OFFSET), event);
#endif
	} else {
		K_ASSERT(0);
	}
}

void vic_enable(u32 line)
{
	u32 r;

	if (line < VIC_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC_REG(VIC_INTEN_OFFSET));
		r |= (0x1 << line);
		writel(VIC_REG(VIC_INTEN_OFFSET), r);
#if (VIC_INSTANCES >= 2)
	} else if (line < VIC2_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC2_REG(VIC_INTEN_OFFSET));
		r |= (0x1 << (line - VIC2_INT_VEC(0)));
		writel(VIC2_REG(VIC_INTEN_OFFSET), r);
#endif
#if (VIC_INSTANCES >= 3)
	} else if (line < VIC3_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC3_REG(VIC_INTEN_OFFSET));
		r |= (0x1 << (line - VIC3_INT_VEC(0)));
		writel(VIC3_REG(VIC_INTEN_OFFSET), r);
#endif
#if (VIC_INSTANCES >= 4)
	} else if (line < VIC4_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC4_REG(VIC_INTEN_OFFSET));
		r |= (0x1 << (line - VIC4_INT_VEC(0)));
		writel(VIC4_REG(VIC_INTEN_OFFSET), r);
#endif
	}
}

void vic_disable(u32 line)
{
	if (line < VIC_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC_REG(VIC_INTEN_CLR_OFFSET),
			(0x1 << line));
#if (VIC_INSTANCES >= 2)
	} else if (line < VIC2_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC2_REG(VIC_INTEN_CLR_OFFSET),
			(0x1 << (line - VIC2_INT_VEC(0))));
#endif
#if (VIC_INSTANCES >= 3)
	} else if (line < VIC3_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC3_REG(VIC_INTEN_CLR_OFFSET),
			(0x1 << (line - VIC3_INT_VEC(0))));
#endif
#if (VIC_INSTANCES >= 4)
	} else if (line < VIC4_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC4_REG(VIC_INTEN_CLR_OFFSET),
			(0x1 << (line - VIC4_INT_VEC(0))));
#endif
	}
}

void vic_ackint(u32 line)
{
	if (line < VIC_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC_REG(VIC_EDGE_CLR_OFFSET),
			(0x1 << line));
#if (VIC_INSTANCES >= 2)
	} else if (line < VIC2_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC2_REG(VIC_EDGE_CLR_OFFSET),
			(0x1 << (line - VIC2_INT_VEC(0))));
#endif
#if (VIC_INSTANCES >= 3)
	} else if (line < VIC3_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC3_REG(VIC_EDGE_CLR_OFFSET),
			(0x1 << (line - VIC3_INT_VEC(0))));
#endif
#if (VIC_INSTANCES >= 4)
	} else if (line < VIC4_INT_VEC(NR_VIC_IRQ_SIZE)) {
		writel(VIC4_REG(VIC_EDGE_CLR_OFFSET),
			(0x1 << (line - VIC4_INT_VEC(0))));
#endif
	}
}

void vic_sw_set(u32 line)
{
	u32 r;

	if (line < VIC_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC_REG(VIC_SOFTEN_OFFSET));
		r |= (0x1 << line);
		writel(VIC_REG(VIC_SOFTEN_OFFSET), r);
#if (VIC_INSTANCES >= 2)
	} else if (line < VIC2_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC2_REG(VIC_SOFTEN_OFFSET));
		r |= (0x1 << (line - VIC2_INT_VEC(0)));
		writel(VIC2_REG(VIC_SOFTEN_OFFSET), r);
#endif
#if (VIC_INSTANCES >= 3)
	} else if (line < VIC3_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC3_REG(VIC_SOFTEN_OFFSET));
		r |= (0x1 << (line - VIC3_INT_VEC(0)));
		writel(VIC3_REG(VIC_SOFTEN_OFFSET), r);
#endif
#if (VIC_INSTANCES >= 4)
	} else if (line < VIC4_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC4_REG(VIC_SOFTEN_OFFSET));
		r |= (0x1 << (line - VIC4_INT_VEC(0)));
		writel(VIC4_REG(VIC_SOFTEN_OFFSET), r);
#endif
	}
}

void vic_sw_clr(u32 line)
{
	u32 r;

	if (line < VIC_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC_REG(VIC_SOFTEN_CLR_OFFSET));
		r |= (0x1 << line);
		writel(VIC_REG(VIC_SOFTEN_CLR_OFFSET), r);
#if (VIC_INSTANCES >= 2)
	} else if (line < VIC2_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC2_REG(VIC_SOFTEN_CLR_OFFSET));
		r |= (0x1 << (line - VIC2_INT_VEC(0)));
		writel(VIC2_REG(VIC_SOFTEN_CLR_OFFSET), r);
#endif
#if (VIC_INSTANCES >= 3)
	} else if (line < VIC3_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC3_REG(VIC_SOFTEN_CLR_OFFSET));
		r |= (0x1 << (line - VIC3_INT_VEC(0)));
		writel(VIC3_REG(VIC_SOFTEN_CLR_OFFSET), r);
#endif
#if (VIC_INSTANCES >= 4)
	} else if (line < VIC4_INT_VEC(NR_VIC_IRQ_SIZE)) {
		r = readl(VIC4_REG(VIC_SOFTEN_CLR_OFFSET));
		r |= (0x1 << (line - VIC4_INT_VEC(0)));
		writel(VIC4_REG(VIC_SOFTEN_CLR_OFFSET), r);
#endif
	}
}
