/*
 *  drivers/cpufreq/cpufreq_nightmare.c
 *
 *  Copyright (C)  2011 Samsung Electronics co. ltd
 *    ByungChang Cha <bc.cha@samsung.com>
 *
 *  Based on ondemand governor
 *  Copyright (C)  2001 Russell King
 *            (C)  2003 Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>.
 *                      Jun Nakajima <jun.nakajima@intel.com>
<<<<<<< HEAD
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
=======
 *  Copyright (C) 2014 Kyriacos Elpidorou [for the Note 4 Adaptation]
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
>>>>>>> 9cbee03... Drivers: CPU Governors
 * Created by Alucard_24@xda
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>
#include <linux/mutex.h>
#include <linux/hrtimer.h>
#include <linux/tick.h>
#include <linux/ktime.h>
#include <linux/sched.h>
#include <linux/slab.h>
<<<<<<< HEAD

#define MIN_SAMPLING_RATE	10000

=======
>>>>>>> 9cbee03... Drivers: CPU Governors
/*
 * dbs is used in this file as a shortform for demandbased switching
 * It helps to keep variable names smaller, simpler
 */

static void do_nightmare_timer(struct work_struct *work);
<<<<<<< HEAD

struct cpufreq_nightmare_cpuinfo {
	u64 prev_cpu_wall;
	u64 prev_cpu_idle;
	struct cpufreq_frequency_table *freq_table;
	struct delayed_work work;
	struct cpufreq_policy *cur_policy;
	bool governor_enabled;
	unsigned int cpu;
=======
static int cpufreq_governor_nightmare(struct cpufreq_policy *policy,
				unsigned int event);

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_NIGHTMARE
static
#endif
struct cpufreq_governor cpufreq_gov_nightmare = {
	.name                   = "nightmare",
	.governor               = cpufreq_governor_nightmare,
	.owner                  = THIS_MODULE,
};

struct cpufreq_nightmare_cpuinfo {
	cputime64_t prev_cpu_wall;
	cputime64_t prev_cpu_idle;
	struct cpufreq_frequency_table *freq_table;
	struct delayed_work work;
	struct cpufreq_policy *cur_policy;
	int cpu;
	unsigned int enable:1;
>>>>>>> 9cbee03... Drivers: CPU Governors
	/*
	 * mutex that serializes governor limit change with
	 * do_nightmare_timer invocation. We do not want do_nightmare_timer to run
	 * when user is changing the governor or limits.
	 */
	struct mutex timer_mutex;
};

static DEFINE_PER_CPU(struct cpufreq_nightmare_cpuinfo, od_nightmare_cpuinfo);

static unsigned int nightmare_enable;	/* number of CPUs using this policy */
/*
 * nightmare_mutex protects nightmare_enable in governor start/stop.
 */
static DEFINE_MUTEX(nightmare_mutex);

<<<<<<< HEAD
/* nightmare tuners */
static struct nightmare_tuners {
	unsigned int sampling_rate;
	int inc_cpu_load_at_min_freq;
	int inc_cpu_load;
	int dec_cpu_load;
	int freq_for_responsiveness;
	int freq_for_responsiveness_max;
	int freq_up_brake_at_min_freq;
	int freq_up_brake;
	int freq_step_at_min_freq;
	int freq_step;
	int freq_step_dec;
	int freq_step_dec_at_max_freq;
	unsigned int io_is_busy;

} nightmare_tuners_ins = {
	.sampling_rate = 50000,
	.inc_cpu_load_at_min_freq = 40,
	.inc_cpu_load = 60,
	.dec_cpu_load = 60,
#ifdef CONFIG_MACH_LGE
	.freq_for_responsiveness = 1728000,
	.freq_for_responsiveness_max = 2265600,
#else
	.freq_for_responsiveness = 1566000,
	.freq_for_responsiveness_max = 1890000,
#endif
	.freq_step_at_min_freq = 40,
	.freq_step = 50,
	.freq_up_brake_at_min_freq = 40,
	.freq_up_brake = 30,
	.freq_step_dec = 10,
	.freq_step_dec_at_max_freq = 10,
	.io_is_busy = 0,
=======
/*static atomic_t min_freq_limit[NR_CPUS];
static atomic_t max_freq_limit[NR_CPUS];*/

/* nightmare tuners */
static struct nightmare_tuners {
	atomic_t sampling_rate;
	atomic_t inc_cpu_load_at_min_freq;
	atomic_t inc_cpu_load;
	atomic_t dec_cpu_load;
	atomic_t freq_for_responsiveness;
	atomic_t freq_for_responsiveness_max;
	atomic_t freq_up_brake_at_min_freq;
	atomic_t freq_up_brake;
	atomic_t freq_step_at_min_freq;
	atomic_t freq_step;
	atomic_t freq_step_dec;
	atomic_t freq_step_dec_at_max_freq;
#ifdef CONFIG_CPU_EXYNOS4210
	atomic_t up_sf_step;
	atomic_t down_sf_step;
#endif
} nightmare_tuners_ins = {
	.sampling_rate = ATOMIC_INIT(60000),
	.inc_cpu_load_at_min_freq = ATOMIC_INIT(60),
	.inc_cpu_load = ATOMIC_INIT(70),
	.dec_cpu_load = ATOMIC_INIT(50),
#ifdef CONFIG_CPU_EXYNOS4210
	.freq_for_responsiveness = ATOMIC_INIT(200000),
	.freq_for_responsiveness_max = ATOMIC_INIT(1200000),
#else
	.freq_for_responsiveness = ATOMIC_INIT(540000),
	.freq_for_responsiveness_max = ATOMIC_INIT(1890000),
#endif
	.freq_step_at_min_freq = ATOMIC_INIT(20),
	.freq_step = ATOMIC_INIT(20),
	.freq_up_brake_at_min_freq = ATOMIC_INIT(30),
	.freq_up_brake = ATOMIC_INIT(30),
	.freq_step_dec = ATOMIC_INIT(10),
	.freq_step_dec_at_max_freq = ATOMIC_INIT(10),
#ifdef CONFIG_CPU_EXYNOS4210
	.up_sf_step = ATOMIC_INIT(0),
	.down_sf_step = ATOMIC_INIT(0),
#endif
>>>>>>> 9cbee03... Drivers: CPU Governors
};

/************************** sysfs interface ************************/

/* cpufreq_nightmare Governor Tunables */
#define show_one(file_name, object)					\
static ssize_t show_##file_name						\
(struct kobject *kobj, struct attribute *attr, char *buf)		\
{									\
<<<<<<< HEAD
	return sprintf(buf, "%d\n", nightmare_tuners_ins.object);		\
=======
	return sprintf(buf, "%d\n", atomic_read(&nightmare_tuners_ins.object));		\
>>>>>>> 9cbee03... Drivers: CPU Governors
}
show_one(sampling_rate, sampling_rate);
show_one(inc_cpu_load_at_min_freq, inc_cpu_load_at_min_freq);
show_one(inc_cpu_load, inc_cpu_load);
show_one(dec_cpu_load, dec_cpu_load);
show_one(freq_for_responsiveness, freq_for_responsiveness);
show_one(freq_for_responsiveness_max, freq_for_responsiveness_max);
show_one(freq_step_at_min_freq, freq_step_at_min_freq);
show_one(freq_step, freq_step);
show_one(freq_up_brake_at_min_freq, freq_up_brake_at_min_freq);
show_one(freq_up_brake, freq_up_brake);
show_one(freq_step_dec, freq_step_dec);
show_one(freq_step_dec_at_max_freq, freq_step_dec_at_max_freq);
<<<<<<< HEAD
show_one(io_is_busy, io_is_busy);
=======
#ifdef CONFIG_CPU_EXYNOS4210
show_one(up_sf_step, up_sf_step);
show_one(down_sf_step, down_sf_step);
#endif

/*#define show_freqlimit_param(file_name, cpu)		\
static ssize_t show_##file_name##_##cpu		\
(struct kobject *kobj, struct attribute *attr, char *buf)		\
{									\
	return sprintf(buf, "%d\n", atomic_read(&file_name[cpu]));	\
}

#define store_freqlimit_param(file_name, cpu)		\
static ssize_t store_##file_name##_##cpu		\
(struct kobject *kobj, struct attribute *attr,				\
	const char *buf, size_t count)					\
{									\
	unsigned int input;						\
	int ret;							\
	ret = sscanf(buf, "%d", &input);				\
	if (ret != 1)							\
		return -EINVAL;						\
	if (input == atomic_read(&file_name[cpu])) {		\
		return count;	\
	}	\
	atomic_set(&file_name[cpu], input);			\
	return count;							\
}*/

/* min freq limit for awaking */
/*show_freqlimit_param(min_freq_limit, 0);
show_freqlimit_param(min_freq_limit, 1);
#if NR_CPUS >= 4
show_freqlimit_param(min_freq_limit, 2);
show_freqlimit_param(min_freq_limit, 3);
#endif*/
/* max freq limit for awaking */
/*show_freqlimit_param(max_freq_limit, 0);
show_freqlimit_param(max_freq_limit, 1);
#if NR_CPUS >= 4
show_freqlimit_param(max_freq_limit, 2);
show_freqlimit_param(max_freq_limit, 3);
#endif*/
/* min freq limit for awaking */
/*store_freqlimit_param(min_freq_limit, 0);
store_freqlimit_param(min_freq_limit, 1);
#if NR_CPUS >= 4
store_freqlimit_param(min_freq_limit, 2);
store_freqlimit_param(min_freq_limit, 3);
#endif*/
/* max freq limit for awaking */
/*store_freqlimit_param(max_freq_limit, 0);
store_freqlimit_param(max_freq_limit, 1);
#if NR_CPUS >= 4
store_freqlimit_param(max_freq_limit, 2);
store_freqlimit_param(max_freq_limit, 3);
#endif
define_one_global_rw(min_freq_limit_0);
define_one_global_rw(min_freq_limit_1);
#if NR_CPUS >= 4
define_one_global_rw(min_freq_limit_2);
define_one_global_rw(min_freq_limit_3);
#endif
define_one_global_rw(max_freq_limit_0);
define_one_global_rw(max_freq_limit_1);
#if NR_CPUS >= 4
define_one_global_rw(max_freq_limit_2);
define_one_global_rw(max_freq_limit_3);
#endif*/

/**
 * update_sampling_rate - update sampling rate effective immediately if needed.
 * @new_rate: new sampling rate
 *
 * If new rate is smaller than the old, simply updaing
 * nightmare_tuners_ins.sampling_rate might not be appropriate. For example,
 * if the original sampling_rate was 1 second and the requested new sampling
 * rate is 10 ms because the user needs immediate reaction from ondemand
 * governor, but not sure if higher frequency will be required or not,
 * then, the governor may change the sampling rate too late; up to 1 second
 * later. Thus, if we are reducing the sampling rate, we need to make the
 * new value effective immediately.
 */
static void update_sampling_rate(unsigned int new_rate)
{
	int cpu;

	atomic_set(&nightmare_tuners_ins.sampling_rate,new_rate);

	get_online_cpus();
	for_each_online_cpu(cpu) {
		struct cpufreq_policy *policy;
		struct cpufreq_nightmare_cpuinfo *nightmare_cpuinfo;
		unsigned long next_sampling, appointed_at;

		policy = cpufreq_cpu_get(cpu);
		if (!policy)
			continue;
		nightmare_cpuinfo = &per_cpu(od_nightmare_cpuinfo, policy->cpu);
		cpufreq_cpu_put(policy);

		mutex_lock(&nightmare_cpuinfo->timer_mutex);

		if (!delayed_work_pending(&nightmare_cpuinfo->work)) {
			mutex_unlock(&nightmare_cpuinfo->timer_mutex);
			continue;
		}

		next_sampling  = jiffies + usecs_to_jiffies(new_rate);
		appointed_at = nightmare_cpuinfo->work.timer.expires;


		if (time_before(next_sampling, appointed_at)) {

			mutex_unlock(&nightmare_cpuinfo->timer_mutex);
			cancel_delayed_work_sync(&nightmare_cpuinfo->work);
			mutex_lock(&nightmare_cpuinfo->timer_mutex);

			#ifdef CONFIG_CPU_EXYNOS4210
				mod_delayed_work_on(nightmare_cpuinfo->cpu, system_wq, &nightmare_cpuinfo->work, usecs_to_jiffies(new_rate));
			#else
				queue_delayed_work_on(nightmare_cpuinfo->cpu, system_wq, &nightmare_cpuinfo->work, usecs_to_jiffies(new_rate));
			#endif
		}
		mutex_unlock(&nightmare_cpuinfo->timer_mutex);
	}
	put_online_cpus();
}
>>>>>>> 9cbee03... Drivers: CPU Governors

/* sampling_rate */
static ssize_t store_sampling_rate(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	int input;
<<<<<<< HEAD
	int ret = 0;
	int mpd = strcmp(current->comm, "mpdecision");

	if (mpd == 0)
		return ret;

	ret = sscanf(buf, "%u", &input);
=======
	int ret;

	ret = sscanf(buf, "%d", &input);
>>>>>>> 9cbee03... Drivers: CPU Governors
	if (ret != 1)
		return -EINVAL;

	input = max(input,10000);
<<<<<<< HEAD

	if (input == nightmare_tuners_ins.sampling_rate)
		return count;

	nightmare_tuners_ins.sampling_rate = input;
=======
	
	if (input == atomic_read(&nightmare_tuners_ins.sampling_rate))
		return count;

	update_sampling_rate(input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* inc_cpu_load_at_min_freq */
static ssize_t store_inc_cpu_load_at_min_freq(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1) {
		return -EINVAL;
	}

<<<<<<< HEAD
	input = min(input,nightmare_tuners_ins.inc_cpu_load);

	if (input == nightmare_tuners_ins.inc_cpu_load_at_min_freq)
		return count;

	nightmare_tuners_ins.inc_cpu_load_at_min_freq = input;
=======
	input = min(input,atomic_read(&nightmare_tuners_ins.inc_cpu_load));

	if (input == atomic_read(&nightmare_tuners_ins.inc_cpu_load_at_min_freq))
		return count;

	atomic_set(&nightmare_tuners_ins.inc_cpu_load_at_min_freq,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* inc_cpu_load */
static ssize_t store_inc_cpu_load(struct kobject *a, struct attribute *b,
					const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,100),0);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.inc_cpu_load)
		return count;

	nightmare_tuners_ins.inc_cpu_load = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.inc_cpu_load))
		return count;

	atomic_set(&nightmare_tuners_ins.inc_cpu_load,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* dec_cpu_load */
static ssize_t store_dec_cpu_load(struct kobject *a, struct attribute *b,
					const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,95),5);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.dec_cpu_load)
		return count;

	nightmare_tuners_ins.dec_cpu_load = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.dec_cpu_load))
		return count;

	atomic_set(&nightmare_tuners_ins.dec_cpu_load,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_for_responsiveness */
static ssize_t store_freq_for_responsiveness(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_for_responsiveness)
		return count;

	nightmare_tuners_ins.freq_for_responsiveness = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_for_responsiveness))
		return count;

	atomic_set(&nightmare_tuners_ins.freq_for_responsiveness,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_for_responsiveness_max */
static ssize_t store_freq_for_responsiveness_max(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_for_responsiveness_max)
		return count;

	nightmare_tuners_ins.freq_for_responsiveness_max = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_for_responsiveness_max))
		return count;

	atomic_set(&nightmare_tuners_ins.freq_for_responsiveness_max,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_step_at_min_freq */
static ssize_t store_freq_step_at_min_freq(struct kobject *a, struct attribute *b,
			       const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,100),0);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_step_at_min_freq)
		return count;

	nightmare_tuners_ins.freq_step_at_min_freq = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_step_at_min_freq))
		return count;

	atomic_set(&nightmare_tuners_ins.freq_step_at_min_freq,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_step */
static ssize_t store_freq_step(struct kobject *a, struct attribute *b,
			       const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,100),0);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_step)
		return count;

	nightmare_tuners_ins.freq_step = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_step))
		return count;

	atomic_set(&nightmare_tuners_ins.freq_step,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_up_brake_at_min_freq */
static ssize_t store_freq_up_brake_at_min_freq(struct kobject *a, struct attribute *b,
				      const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,100),0);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_up_brake_at_min_freq)
		return count;

	nightmare_tuners_ins.freq_up_brake_at_min_freq = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_up_brake_at_min_freq)) {/* nothing to do */
		return count;
	}

	atomic_set(&nightmare_tuners_ins.freq_up_brake_at_min_freq,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_up_brake */
static ssize_t store_freq_up_brake(struct kobject *a, struct attribute *b,
				      const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,100),0);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_up_brake)
		return count;

	nightmare_tuners_ins.freq_up_brake = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_up_brake)) {/* nothing to do */
		return count;
	}

	atomic_set(&nightmare_tuners_ins.freq_up_brake,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_step_dec */
static ssize_t store_freq_step_dec(struct kobject *a, struct attribute *b,
				       const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,100),0);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_step_dec)
		return count;

	nightmare_tuners_ins.freq_step_dec = input;
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_step_dec)) {/* nothing to do */
		return count;
	}

	atomic_set(&nightmare_tuners_ins.freq_step_dec,input);
>>>>>>> 9cbee03... Drivers: CPU Governors

	return count;
}

/* freq_step_dec_at_max_freq */
static ssize_t store_freq_step_dec_at_max_freq(struct kobject *a, struct attribute *b,
				       const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,100),0);

<<<<<<< HEAD
	if (input == nightmare_tuners_ins.freq_step_dec_at_max_freq)
		return count;

	nightmare_tuners_ins.freq_step_dec_at_max_freq = input;

	return count;
}

/* io_is_busy */
static ssize_t store_io_is_busy(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	unsigned int input, cpu;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	if (input > 1)
		input = 1;

	if (input == nightmare_tuners_ins.io_is_busy)
		return count;

	nightmare_tuners_ins.io_is_busy = !!input;

	/* we need to re-evaluate prev_cpu_idle */
	get_online_cpus();
	for_each_online_cpu(cpu) {
		struct cpufreq_nightmare_cpuinfo *this_nightmare_cpuinfo = 
			&per_cpu(od_nightmare_cpuinfo, cpu);

		this_nightmare_cpuinfo->prev_cpu_idle = get_cpu_idle_time(cpu,
			&this_nightmare_cpuinfo->prev_cpu_wall, nightmare_tuners_ins.io_is_busy);
	}
	put_online_cpus();
	return count;
}
=======
	if (input == atomic_read(&nightmare_tuners_ins.freq_step_dec_at_max_freq)) {/* nothing to do */
		return count;
	}

	atomic_set(&nightmare_tuners_ins.freq_step_dec_at_max_freq,input);

	return count;
}
#ifdef CONFIG_CPU_EXYNOS4210
/* up_sf_step */
static ssize_t store_up_sf_step(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,99),0);

	if (input == atomic_read(&nightmare_tuners_ins.up_sf_step))
		return count;

	 atomic_set(&nightmare_tuners_ins.up_sf_step,input);

	return count;
}

/* down_sf_step */
static ssize_t store_down_sf_step(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	int input;
	int ret;

	ret = sscanf(buf, "%d", &input);
	if (ret != 1)
		return -EINVAL;

	input = max(min(input,99),0);

	if (input == atomic_read(&nightmare_tuners_ins.down_sf_step))
		return count;

	atomic_set(&nightmare_tuners_ins.down_sf_step,input);

	return count;
}
#endif
>>>>>>> 9cbee03... Drivers: CPU Governors

define_one_global_rw(sampling_rate);
define_one_global_rw(inc_cpu_load_at_min_freq);
define_one_global_rw(inc_cpu_load);
define_one_global_rw(dec_cpu_load);
define_one_global_rw(freq_for_responsiveness);
define_one_global_rw(freq_for_responsiveness_max);
define_one_global_rw(freq_step_at_min_freq);
define_one_global_rw(freq_step);
define_one_global_rw(freq_up_brake_at_min_freq);
define_one_global_rw(freq_up_brake);
define_one_global_rw(freq_step_dec);
define_one_global_rw(freq_step_dec_at_max_freq);
<<<<<<< HEAD
define_one_global_rw(io_is_busy);

static struct attribute *nightmare_attributes[] = {
	&sampling_rate.attr,
=======
#ifdef CONFIG_CPU_EXYNOS4210
define_one_global_rw(up_sf_step);
define_one_global_rw(down_sf_step);
#endif

static struct attribute *nightmare_attributes[] = {
	&sampling_rate.attr,
	/*&min_freq_limit_0.attr,
	&min_freq_limit_1.attr,
#if NR_CPUS >= 4
	&min_freq_limit_2.attr,
	&min_freq_limit_3.attr,
#endif
	&max_freq_limit_0.attr,
	&max_freq_limit_1.attr,
#if NR_CPUS >= 4
	&max_freq_limit_2.attr,
	&max_freq_limit_3.attr,
#endif*/
>>>>>>> 9cbee03... Drivers: CPU Governors
	&inc_cpu_load_at_min_freq.attr,
	&inc_cpu_load.attr,
	&dec_cpu_load.attr,
	&freq_for_responsiveness.attr,
	&freq_for_responsiveness_max.attr,
	&freq_step_at_min_freq.attr,
	&freq_step.attr,
	&freq_up_brake_at_min_freq.attr,
	&freq_up_brake.attr,
	&freq_step_dec.attr,
	&freq_step_dec_at_max_freq.attr,
<<<<<<< HEAD
	&io_is_busy.attr,
=======
#ifdef CONFIG_CPU_EXYNOS4210
	&up_sf_step.attr,
	&down_sf_step.attr,
#endif
>>>>>>> 9cbee03... Drivers: CPU Governors
	NULL
};

static struct attribute_group nightmare_attr_group = {
	.attrs = nightmare_attributes,
	.name = "nightmare",
};

/************************** sysfs end ************************/

<<<<<<< HEAD
static unsigned int adjust_cpufreq_frequency_target(struct cpufreq_policy *policy,
					struct cpufreq_frequency_table *table,
					unsigned int tmp_freq)
{
	unsigned int i = 0, l_freq = 0, h_freq = 0, target_freq = 0;

	if (tmp_freq < policy->min)
		tmp_freq = policy->min;
	if (tmp_freq > policy->max)
		tmp_freq = policy->max;

	for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++) {
		unsigned int freq = table[i].frequency;
		if (freq == CPUFREQ_ENTRY_INVALID) {
			continue;
		}
		if (freq < tmp_freq) {
			h_freq = freq;
		}
		if (freq == tmp_freq) {
			target_freq = freq;
			break;
		}
		if (freq > tmp_freq) {
			l_freq = freq;
			break;
		}
	}
	if (!target_freq) {
		if (policy->cur >= h_freq
			 && policy->cur <= l_freq)
			target_freq = policy->cur;
		else
			target_freq = l_freq;
	}

	return target_freq;
}

static void nightmare_check_cpu(struct cpufreq_nightmare_cpuinfo *this_nightmare_cpuinfo)
{
	struct cpufreq_policy *policy;
	unsigned int freq_for_responsiveness = nightmare_tuners_ins.freq_for_responsiveness;
	unsigned int freq_for_responsiveness_max = nightmare_tuners_ins.freq_for_responsiveness_max;
	int dec_cpu_load = nightmare_tuners_ins.dec_cpu_load;
	int inc_cpu_load = nightmare_tuners_ins.inc_cpu_load;
	int freq_step = nightmare_tuners_ins.freq_step;
	int freq_up_brake = nightmare_tuners_ins.freq_up_brake;
	int freq_step_dec = nightmare_tuners_ins.freq_step_dec;
	unsigned int max_load = 0;
	unsigned int tmp_freq = 0;
	int io_busy = nightmare_tuners_ins.io_is_busy;
	unsigned int cpu = this_nightmare_cpuinfo->cpu;
	unsigned int j;

	policy = this_nightmare_cpuinfo->cur_policy;
	if (!policy->cur)
		return;

	for_each_cpu(j, policy->cpus) {
		struct cpufreq_nightmare_cpuinfo *j_nightmare_cpuinfo;
		u64 cur_wall_time, cur_idle_time;
		unsigned int idle_time, wall_time;
		unsigned int load;
		
		j_nightmare_cpuinfo = &per_cpu(od_nightmare_cpuinfo, j);

		if (!j_nightmare_cpuinfo->governor_enabled)
			continue;

		cur_idle_time = get_cpu_idle_time(j, &cur_wall_time, io_busy);

		wall_time = (unsigned int)
			(cur_wall_time - j_nightmare_cpuinfo->prev_cpu_wall);
		idle_time = (unsigned int)
			(cur_idle_time - j_nightmare_cpuinfo->prev_cpu_idle);

		if (j == cpu) {
			j_nightmare_cpuinfo->prev_cpu_wall = cur_wall_time;
			j_nightmare_cpuinfo->prev_cpu_idle = cur_idle_time;
		}

		if (unlikely(!wall_time || wall_time < idle_time))
			continue;

		load = 100 * (wall_time - idle_time) / wall_time;

		if (load > max_load)
			max_load = load;
	}

	cpufreq_notify_utilization(policy, max_load);

	/* CPUs Online Scale Frequency*/
	if (policy->cur < freq_for_responsiveness) {
		inc_cpu_load = nightmare_tuners_ins.inc_cpu_load_at_min_freq;
		freq_step = nightmare_tuners_ins.freq_step_at_min_freq;
		freq_up_brake = nightmare_tuners_ins.freq_up_brake_at_min_freq;
	} else if (policy->cur > freq_for_responsiveness_max) {
		freq_step_dec = nightmare_tuners_ins.freq_step_dec_at_max_freq;
	}
	/* Check for frequency increase or for frequency decrease */
	if (max_load >= inc_cpu_load && policy->cur < policy->max) {
		tmp_freq = adjust_cpufreq_frequency_target(policy, this_nightmare_cpuinfo->freq_table,
												   (policy->cur + ((max_load + freq_step - freq_up_brake == 0 ? 1 : max_load + freq_step - freq_up_brake) * 3780)));
	} else if (max_load < dec_cpu_load && policy->cur > policy->min) {
		tmp_freq = adjust_cpufreq_frequency_target(policy, this_nightmare_cpuinfo->freq_table,
													(policy->cur - ((100 - max_load + freq_step_dec == 0 ? 1 : 100 - max_load + freq_step_dec) * 3780)));
	} else {
		/* if cpu frequency is already at maximum or minimum or max_load is between inc_cpu_load and dec_cpu_load var, we don't need to set frequency! */
		return;
	}
	if (tmp_freq != policy->cur && tmp_freq > 0)
		__cpufreq_driver_target(policy, tmp_freq, CPUFREQ_RELATION_L);
=======
static void nightmare_check_cpu(struct cpufreq_nightmare_cpuinfo *this_nightmare_cpuinfo)
{
	struct cpufreq_policy *cpu_policy;
	unsigned int min_freq;
	unsigned int max_freq;
#ifdef CONFIG_CPU_EXYNOS4210
	int up_sf_step = atomic_read(&nightmare_tuners_ins.up_sf_step);
	int down_sf_step = atomic_read(&nightmare_tuners_ins.down_sf_step);
#endif
	unsigned int freq_for_responsiveness;
	unsigned int freq_for_responsiveness_max;
	int dec_cpu_load;
	int inc_cpu_load;
	int freq_step;
	int freq_up_brake;
	int freq_step_dec;
	cputime64_t cur_wall_time, cur_idle_time;
	unsigned int wall_time, idle_time;
	unsigned int index = 0;
	unsigned int tmp_freq = 0;
	unsigned int next_freq = 0;
	int cur_load = -1;
	unsigned int cpu;
	
	cpu = this_nightmare_cpuinfo->cpu;
	cpu_policy = this_nightmare_cpuinfo->cur_policy;

	cur_idle_time = get_cpu_idle_time_us(cpu, NULL);
	cur_idle_time += get_cpu_iowait_time_us(cpu, &cur_wall_time);

	wall_time = (unsigned int)
			(cur_wall_time - this_nightmare_cpuinfo->prev_cpu_wall);
	this_nightmare_cpuinfo->prev_cpu_wall = cur_wall_time;

	idle_time = (unsigned int)
			(cur_idle_time - this_nightmare_cpuinfo->prev_cpu_idle);
	this_nightmare_cpuinfo->prev_cpu_idle = cur_idle_time;

	/*min_freq = atomic_read(&min_freq_limit[cpu]);
	max_freq = atomic_read(&max_freq_limit[cpu]);*/

	freq_for_responsiveness = atomic_read(&nightmare_tuners_ins.freq_for_responsiveness);
	freq_for_responsiveness_max = atomic_read(&nightmare_tuners_ins.freq_for_responsiveness_max);
	dec_cpu_load = atomic_read(&nightmare_tuners_ins.dec_cpu_load);
	inc_cpu_load = atomic_read(&nightmare_tuners_ins.inc_cpu_load);
	freq_step = atomic_read(&nightmare_tuners_ins.freq_step);
	freq_up_brake = atomic_read(&nightmare_tuners_ins.freq_up_brake);
	freq_step_dec = atomic_read(&nightmare_tuners_ins.freq_step_dec);

	if (!cpu_policy)
		return;

	/*printk(KERN_ERR "TIMER CPU[%u], wall[%u], idle[%u]\n",cpu, wall_time, idle_time);*/
	if (wall_time >= idle_time) { /*if wall_time < idle_time, evaluate cpu load next time*/
		cur_load = wall_time > idle_time ? (100 * (wall_time - idle_time)) / wall_time : 1;/*if wall_time is equal to idle_time cpu_load is equal to 1*/
		tmp_freq = cpu_policy->cur;
		/* Checking Frequency Limit */
		/*if (max_freq > cpu_policy->max)
			max_freq = cpu_policy->max;
		if (min_freq < cpu_policy->min)
			min_freq = cpu_policy->min;*/
		min_freq = cpu_policy->min;
		max_freq = cpu_policy->max;		
		/* CPUs Online Scale Frequency*/
		if (cpu_policy->cur < freq_for_responsiveness) {
			inc_cpu_load = atomic_read(&nightmare_tuners_ins.inc_cpu_load_at_min_freq);
			freq_step = atomic_read(&nightmare_tuners_ins.freq_step_at_min_freq);
			freq_up_brake = atomic_read(&nightmare_tuners_ins.freq_up_brake_at_min_freq);
		} else if (cpu_policy->cur > freq_for_responsiveness_max) {
			freq_step_dec = atomic_read(&nightmare_tuners_ins.freq_step_dec_at_max_freq);
		}		
		/* Check for frequency increase or for frequency decrease */
#ifdef CONFIG_CPU_EXYNOS4210
		if (cur_load >= inc_cpu_load && cpu_policy->cur < max_freq) {
			tmp_freq = max(min((cpu_policy->cur + ((cur_load + freq_step - freq_up_brake == 0 ? 1 : cur_load + freq_step - freq_up_brake) * 2000)), max_freq), min_freq);
		} else if (cur_load < dec_cpu_load && cpu_policy->cur > min_freq) {
			tmp_freq = max(min((cpu_policy->cur - ((100 - cur_load + freq_step_dec == 0 ? 1 : 100 - cur_load + freq_step_dec) * 2000)), max_freq), min_freq);
		}
		next_freq = (tmp_freq / 100000) * 100000;
		if ((next_freq > cpu_policy->cur
			&& (tmp_freq % 100000 > up_sf_step * 1000))
			|| (next_freq < cpu_policy->cur
			&& (tmp_freq % 100000 > down_sf_step * 1000))) {
				next_freq += 100000;
		}
#else
		if (cur_load >= inc_cpu_load && cpu_policy->cur < max_freq) {
			tmp_freq = max(min((cpu_policy->cur + ((cur_load + freq_step - freq_up_brake == 0 ? 1 : cur_load + freq_step - freq_up_brake) * 3840)), max_freq), min_freq);
		} else if (cur_load < dec_cpu_load && cpu_policy->cur > min_freq) {
			tmp_freq = max(min((cpu_policy->cur - ((100 - cur_load + freq_step_dec == 0 ? 1 : 100 - cur_load + freq_step_dec) * 3840)), max_freq), min_freq);
		}
		cpufreq_frequency_table_target(cpu_policy, this_nightmare_cpuinfo->freq_table, tmp_freq,
			CPUFREQ_RELATION_H, &index);
		if (this_nightmare_cpuinfo->freq_table[index].frequency != cpu_policy->cur) {
			cpufreq_frequency_table_target(cpu_policy, this_nightmare_cpuinfo->freq_table, tmp_freq,
				CPUFREQ_RELATION_L, &index);
		}
	 	next_freq = this_nightmare_cpuinfo->freq_table[index].frequency;
#endif
		/*printk(KERN_ERR "FREQ CALC.: CPU[%u], load[%d], target freq[%u], cur freq[%u], min freq[%u], max_freq[%u]\n",cpu, cur_load, next_freq, cpu_policy->cur, cpu_policy->min, max_freq);*/
		if (next_freq != cpu_policy->cur && cpu_online(cpu)) {
			__cpufreq_driver_target(cpu_policy, next_freq, CPUFREQ_RELATION_L);
		}
	}

>>>>>>> 9cbee03... Drivers: CPU Governors
}

static void do_nightmare_timer(struct work_struct *work)
{
<<<<<<< HEAD
	struct cpufreq_nightmare_cpuinfo *this_nightmare_cpuinfo =
		container_of(work, struct cpufreq_nightmare_cpuinfo, work.work);
	int delay;
	unsigned int cpu = this_nightmare_cpuinfo->cpu;

	mutex_lock(&this_nightmare_cpuinfo->timer_mutex);

	nightmare_check_cpu(this_nightmare_cpuinfo);

	delay = usecs_to_jiffies(nightmare_tuners_ins.sampling_rate);
	/* We want all CPUs to do sampling nearly on
	 * same jiffy
	 */
	if (num_online_cpus() > 1) {
		delay -= jiffies % delay;
		if (delay < 0)
			delay = 0;
	}

	mod_delayed_work_on(cpu, system_wq,
			&this_nightmare_cpuinfo->work, delay);
	mutex_unlock(&this_nightmare_cpuinfo->timer_mutex);
=======
	struct cpufreq_nightmare_cpuinfo *nightmare_cpuinfo;
	int delay;
	unsigned int cpu;

	nightmare_cpuinfo = container_of(work, struct cpufreq_nightmare_cpuinfo, work.work);
	cpu = nightmare_cpuinfo->cpu;

	mutex_lock(&nightmare_cpuinfo->timer_mutex);
	nightmare_check_cpu(nightmare_cpuinfo);
	/* We want all CPUs to do sampling nearly on
	 * same jiffy
	 */
	delay = usecs_to_jiffies(atomic_read(&nightmare_tuners_ins.sampling_rate));
	if (num_online_cpus() > 1) {
		delay -= jiffies % delay;
	}

#ifdef CONFIG_CPU_EXYNOS4210
	mod_delayed_work_on(cpu, system_wq, &nightmare_cpuinfo->work, delay);
#else
	queue_delayed_work_on(cpu, system_wq, &nightmare_cpuinfo->work, delay);
#endif
	mutex_unlock(&nightmare_cpuinfo->timer_mutex);
>>>>>>> 9cbee03... Drivers: CPU Governors
}

static int cpufreq_governor_nightmare(struct cpufreq_policy *policy,
				unsigned int event)
{
<<<<<<< HEAD
	struct cpufreq_nightmare_cpuinfo *this_nightmare_cpuinfo;
	unsigned int cpu = policy->cpu;
	int io_busy = nightmare_tuners_ins.io_is_busy;
	int rc, delay;

	this_nightmare_cpuinfo = &per_cpu(od_nightmare_cpuinfo, cpu);
	this_nightmare_cpuinfo->cpu = cpu;

	switch (event) {
	case CPUFREQ_GOV_START:
		if (!policy->cur)
			return -EINVAL;

		mutex_lock(&nightmare_mutex);
		this_nightmare_cpuinfo->freq_table = cpufreq_frequency_get_table(cpu);
		if (!this_nightmare_cpuinfo->freq_table) {
			mutex_unlock(&nightmare_mutex);
			return -EINVAL;
		}

		this_nightmare_cpuinfo->cur_policy = policy;

		this_nightmare_cpuinfo->prev_cpu_idle = get_cpu_idle_time(cpu,
			&this_nightmare_cpuinfo->prev_cpu_wall, io_busy);
=======
	unsigned int cpu;
	struct cpufreq_nightmare_cpuinfo *this_nightmare_cpuinfo;
	int rc, delay;

	cpu = policy->cpu;
	this_nightmare_cpuinfo = &per_cpu(od_nightmare_cpuinfo, cpu);

	switch (event) {
	case CPUFREQ_GOV_START:
		if ((!cpu_online(cpu)) || (!policy->cur))
			return -EINVAL;

		mutex_lock(&nightmare_mutex);

		this_nightmare_cpuinfo->cur_policy = policy;

		this_nightmare_cpuinfo->prev_cpu_idle = get_cpu_idle_time_us(cpu, NULL);
		this_nightmare_cpuinfo->prev_cpu_idle += get_cpu_iowait_time_us(cpu, &this_nightmare_cpuinfo->prev_cpu_wall);

		this_nightmare_cpuinfo->freq_table = cpufreq_frequency_get_table(cpu);
		this_nightmare_cpuinfo->cpu = cpu;

		mutex_init(&this_nightmare_cpuinfo->timer_mutex);
>>>>>>> 9cbee03... Drivers: CPU Governors

		nightmare_enable++;
		/*
		 * Start the timerschedule work, when this governor
		 * is used for first time
		 */
		if (nightmare_enable == 1) {
			rc = sysfs_create_group(cpufreq_global_kobject,
						&nightmare_attr_group);
			if (rc) {
<<<<<<< HEAD
				nightmare_enable--;
=======
>>>>>>> 9cbee03... Drivers: CPU Governors
				mutex_unlock(&nightmare_mutex);
				return rc;
			}
		}
<<<<<<< HEAD
		this_nightmare_cpuinfo->governor_enabled = true;
		mutex_unlock(&nightmare_mutex);

		mutex_init(&this_nightmare_cpuinfo->timer_mutex);

		delay=usecs_to_jiffies(nightmare_tuners_ins.sampling_rate);
		/* We want all CPUs to do sampling nearly on same jiffy */
		if (num_online_cpus() > 1) {
			delay -= jiffies % delay;
			if (delay < 0)
				delay = 0;
		}

		INIT_DEFERRABLE_WORK(&this_nightmare_cpuinfo->work, do_nightmare_timer);
		mod_delayed_work_on(cpu,
			system_wq, &this_nightmare_cpuinfo->work, delay);

		break;
	case CPUFREQ_GOV_STOP:
		cancel_delayed_work_sync(&this_nightmare_cpuinfo->work);

		mutex_lock(&nightmare_mutex);
		mutex_destroy(&this_nightmare_cpuinfo->timer_mutex);

		this_nightmare_cpuinfo->governor_enabled = false;

		this_nightmare_cpuinfo->cur_policy = NULL;

		nightmare_enable--;
		if (!nightmare_enable) {
			sysfs_remove_group(cpufreq_global_kobject,
					   &nightmare_attr_group);
		}
		mutex_unlock(&nightmare_mutex);

		break;
	case CPUFREQ_GOV_LIMITS:
		if (!this_nightmare_cpuinfo->cur_policy->cur
			 || !policy->cur) {
			pr_debug("Unable to limit cpu freq due to cur_policy == NULL\n");
			return -EPERM;
		}
		mutex_lock(&this_nightmare_cpuinfo->timer_mutex);
		__cpufreq_driver_target(this_nightmare_cpuinfo->cur_policy,
				policy->cur, CPUFREQ_RELATION_L);
=======

		/*if (atomic_read(&min_freq_limit[cpu]) == 0)
			atomic_set(&min_freq_limit[cpu], policy->min);

		if (atomic_read(&max_freq_limit[cpu]) == 0)
			atomic_set(&max_freq_limit[cpu], policy->max);*/

		mutex_unlock(&nightmare_mutex);

		delay=usecs_to_jiffies(atomic_read(&nightmare_tuners_ins.sampling_rate));
		if (num_online_cpus() > 1) {
			delay -= jiffies % delay;
		}

		this_nightmare_cpuinfo->enable = 1;
#ifdef CONFIG_CPU_EXYNOS4210
		INIT_DEFERRABLE_WORK(&this_nightmare_cpuinfo->work, do_nightmare_timer);
		mod_delayed_work_on(this_nightmare_cpuinfo->cpu, system_wq, &this_nightmare_cpuinfo->work, delay);
#else
		INIT_DEFERRABLE_WORK(&this_nightmare_cpuinfo->work, do_nightmare_timer);
		queue_delayed_work_on(this_nightmare_cpuinfo->cpu, system_wq, &this_nightmare_cpuinfo->work, delay);
#endif

		break;

	case CPUFREQ_GOV_STOP:
		this_nightmare_cpuinfo->enable = 0;
		cancel_delayed_work_sync(&this_nightmare_cpuinfo->work);

		mutex_lock(&nightmare_mutex);
		nightmare_enable--;
		mutex_destroy(&this_nightmare_cpuinfo->timer_mutex);

		if (!nightmare_enable) {
			sysfs_remove_group(cpufreq_global_kobject,
					   &nightmare_attr_group);			
		}
		mutex_unlock(&nightmare_mutex);
		
		break;

	case CPUFREQ_GOV_LIMITS:
		mutex_lock(&this_nightmare_cpuinfo->timer_mutex);
		if (policy->max < this_nightmare_cpuinfo->cur_policy->cur)
			__cpufreq_driver_target(this_nightmare_cpuinfo->cur_policy,
				policy->max, CPUFREQ_RELATION_H);
		else if (policy->min > this_nightmare_cpuinfo->cur_policy->cur)
			__cpufreq_driver_target(this_nightmare_cpuinfo->cur_policy,
				policy->min, CPUFREQ_RELATION_L);
>>>>>>> 9cbee03... Drivers: CPU Governors
		mutex_unlock(&this_nightmare_cpuinfo->timer_mutex);

		break;
	}
	return 0;
}

<<<<<<< HEAD
#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_NIGHTMARE
static
#endif
struct cpufreq_governor cpufreq_gov_nightmare = {
	.name                   = "nightmare",
	.governor               = cpufreq_governor_nightmare,
	.owner                  = THIS_MODULE,
};

=======
>>>>>>> 9cbee03... Drivers: CPU Governors
static int __init cpufreq_gov_nightmare_init(void)
{
	return cpufreq_register_governor(&cpufreq_gov_nightmare);
}

static void __exit cpufreq_gov_nightmare_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_nightmare);
}

MODULE_AUTHOR("Alucard24@XDA");
<<<<<<< HEAD
MODULE_DESCRIPTION("'cpufreq_nightmare' - A dynamic cpufreq/cpuhotplug governor v4.5 (SnapDragon)");
=======
MODULE_DESCRIPTION("'cpufreq_nightmare' - A dynamic cpufreq/cpuhotplug governor v4.1 (SnapDragon)");
>>>>>>> 9cbee03... Drivers: CPU Governors
MODULE_LICENSE("GPL");

#ifdef CONFIG_CPU_FREQ_DEFAULT_GOV_NIGHTMARE
fs_initcall(cpufreq_gov_nightmare_init);
#else
module_init(cpufreq_gov_nightmare_init);
#endif
module_exit(cpufreq_gov_nightmare_exit);
<<<<<<< HEAD
=======

>>>>>>> 9cbee03... Drivers: CPU Governors
