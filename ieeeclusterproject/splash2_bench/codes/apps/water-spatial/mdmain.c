#line 228 "/home/apoorve/project/splash2_bench/codes/null_macros/c.m4.null.POSIX"

#line 1 "mdmain.C"
/*************************************************************************/
/*                                                                       */
/*  Copyright (c) 1994 Stanford University                               */
/*                                                                       */
/*  All rights reserved.                                                 */
/*                                                                       */
/*  Permission is given to use, copy, and modify this software for any   */
/*  non-commercial purpose as long as this copyright notice is not       */
/*  removed.  All other uses, including redistribution in whole or in    */
/*  part, are forbidden without prior written permission.                */
/*                                                                       */
/*  This software is provided with absolutely no warranty and no         */
/*  support.                                                             */
/*                                                                       */
/*************************************************************************/


#line 17
#include <pthread.h>
#line 17
#include <sys/time.h>
#line 17
#include <unistd.h>
#line 17
#include <stdlib.h>
#line 17
extern pthread_t PThreadTable[];
#line 17


#include <stdio.h>

#include "parameters.h"
#include "mdvar.h"
#include "water.h"
#include "wwpot.h"
#include "cnst.h"
#include "mddata.h"
#include "fileio.h"
#include "split.h"
#include "global.h"

/************************************************************************/

double MDMAIN(long NSTEP, long NPRINT, long NSAVE, long NORD1, long ProcID)
{
    double TVIR = 0.0;
    double TTMV = 0.0;
    double TKIN = 0.0;
    double XTT;
    long i,j,k;
    double POTA,POTR,POTRF;
    double XVIR,AVGT,TEN;
    struct list_of_boxes *new_box, *curr_box;

    for (i=start_end[ProcID]->box[XDIR][FIRST]; i<=start_end[ProcID]->box[XDIR][LAST]; i++) {
        for (j=start_end[ProcID]->box[YDIR][FIRST]; j<=start_end[ProcID]->box[YDIR][LAST]; j++) {
            for (k=start_end[ProcID]->box[ZDIR][FIRST]; k<=start_end[ProcID]->box[ZDIR][LAST]; k++) {
//                new_box = (box_list *) valloc(sizeof(box_list));;
			posix_memalign(&new_box, sizeof(box_list*), sizeof(box_list));

                new_box->coord[XDIR] = i;
                new_box->coord[YDIR] = j;
                new_box->coord[ZDIR] = k;
                new_box->next_box = NULL;
                curr_box = my_boxes[ProcID];
                if (curr_box == NULL)
                    my_boxes[ProcID] = new_box;
                else {
                    while (curr_box->next_box != NULL)
                        curr_box = curr_box->next_box;
                    curr_box->next_box = new_box;
                } /* else */
            }
        }
    }

    /* calculate initial value for acceleration */

    INTRAF(&gl->VIR,ProcID);

    {
#line 70
	unsigned long	Error, Cycle;
#line 70
	long		Cancel, Temp;
#line 70

#line 70
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 70
	if (Error != 0) {
#line 70
		printf("Error while trying to get lock in barrier.\n");
#line 70
		exit(-1);
#line 70
	}
#line 70

#line 70
	Cycle = (gl->start).cycle;
#line 70
	if (++(gl->start).counter != (NumProcs)) {
#line 70
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 70
		while (Cycle == (gl->start).cycle) {
#line 70
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 70
			if (Error != 0) {
#line 70
				break;
#line 70
			}
#line 70
		}
#line 70
		pthread_setcancelstate(Cancel, &Temp);
#line 70
	} else {
#line 70
		(gl->start).cycle = !(gl->start).cycle;
#line 70
		(gl->start).counter = 0;
#line 70
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 70
	}
#line 70
	pthread_mutex_unlock(&(gl->start).mutex);
#line 70
};

    INTERF(ACC,&gl->VIR,ProcID);

    {
#line 74
	unsigned long	Error, Cycle;
#line 74
	long		Cancel, Temp;
#line 74

#line 74
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 74
	if (Error != 0) {
#line 74
		printf("Error while trying to get lock in barrier.\n");
#line 74
		exit(-1);
#line 74
	}
#line 74

#line 74
	Cycle = (gl->start).cycle;
#line 74
	if (++(gl->start).counter != (NumProcs)) {
#line 74
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 74
		while (Cycle == (gl->start).cycle) {
#line 74
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 74
			if (Error != 0) {
#line 74
				break;
#line 74
			}
#line 74
		}
#line 74
		pthread_setcancelstate(Cancel, &Temp);
#line 74
	} else {
#line 74
		(gl->start).cycle = !(gl->start).cycle;
#line 74
		(gl->start).counter = 0;
#line 74
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 74
	}
#line 74
	pthread_mutex_unlock(&(gl->start).mutex);
#line 74
};

    /* MOLECULAR DYNAMICS LOOP */

    for (i=1;i <= NSTEP; i++) {
        TTMV=TTMV+1.00;

        /* POSSIBLE ENHANCEMENT:  Here's where one start measurements to avoid
           cold-start effects.  Recommended to do this at the beginning of the
           second timestep; i.e. if (i == 2).
           */

        /* initialize various shared sums */
        if (ProcID == 0) {
            long dir;
            if (i >= 2) {
                {
#line 90
	struct timeval	FullTime;
#line 90

#line 90
	gettimeofday(&FullTime, NULL);
#line 90
	(gl->trackstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 90
};
            }
            gl->VIR = 0.0;
            gl->POTA = 0.0;
            gl->POTR = 0.0;
            gl->POTRF = 0.0;
            for (dir = XDIR; dir <= ZDIR; dir++)
                gl->SUM[dir] = 0.0;
        }

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 101
	struct timeval	FullTime;
#line 101

#line 101
	gettimeofday(&FullTime, NULL);
#line 101
	(gl->intrastart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 101
};
        }

        {
#line 104
	unsigned long	Error, Cycle;
#line 104
	long		Cancel, Temp;
#line 104

#line 104
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 104
	if (Error != 0) {
#line 104
		printf("Error while trying to get lock in barrier.\n");
#line 104
		exit(-1);
#line 104
	}
#line 104

#line 104
	Cycle = (gl->start).cycle;
#line 104
	if (++(gl->start).counter != (NumProcs)) {
#line 104
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 104
		while (Cycle == (gl->start).cycle) {
#line 104
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 104
			if (Error != 0) {
#line 104
				break;
#line 104
			}
#line 104
		}
#line 104
		pthread_setcancelstate(Cancel, &Temp);
#line 104
	} else {
#line 104
		(gl->start).cycle = !(gl->start).cycle;
#line 104
		(gl->start).counter = 0;
#line 104
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 104
	}
#line 104
	pthread_mutex_unlock(&(gl->start).mutex);
#line 104
};

        PREDIC(TLC,NORD1,ProcID);
        INTRAF(&gl->VIR,ProcID);

        {
#line 109
	unsigned long	Error, Cycle;
#line 109
	long		Cancel, Temp;
#line 109

#line 109
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 109
	if (Error != 0) {
#line 109
		printf("Error while trying to get lock in barrier.\n");
#line 109
		exit(-1);
#line 109
	}
#line 109

#line 109
	Cycle = (gl->start).cycle;
#line 109
	if (++(gl->start).counter != (NumProcs)) {
#line 109
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 109
		while (Cycle == (gl->start).cycle) {
#line 109
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 109
			if (Error != 0) {
#line 109
				break;
#line 109
			}
#line 109
		}
#line 109
		pthread_setcancelstate(Cancel, &Temp);
#line 109
	} else {
#line 109
		(gl->start).cycle = !(gl->start).cycle;
#line 109
		(gl->start).counter = 0;
#line 109
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 109
	}
#line 109
	pthread_mutex_unlock(&(gl->start).mutex);
#line 109
};

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 112
	struct timeval	FullTime;
#line 112

#line 112
	gettimeofday(&FullTime, NULL);
#line 112
	(gl->intraend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 112
};
            gl->intratime += gl->intraend - gl->intrastart;
        }

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 117
	struct timeval	FullTime;
#line 117

#line 117
	gettimeofday(&FullTime, NULL);
#line 117
	(gl->interstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 117
};
        }

        INTERF(FORCES,&gl->VIR,ProcID);

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 123
	struct timeval	FullTime;
#line 123

#line 123
	gettimeofday(&FullTime, NULL);
#line 123
	(gl->interend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 123
};
            gl->intertime += gl->interend - gl->interstart;
        }

        CORREC(PCC,NORD1,ProcID);

        BNDRY(ProcID);

        KINETI(gl->SUM,HMAS,OMAS,ProcID);

        {
#line 133
	unsigned long	Error, Cycle;
#line 133
	long		Cancel, Temp;
#line 133

#line 133
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 133
	if (Error != 0) {
#line 133
		printf("Error while trying to get lock in barrier.\n");
#line 133
		exit(-1);
#line 133
	}
#line 133

#line 133
	Cycle = (gl->start).cycle;
#line 133
	if (++(gl->start).counter != (NumProcs)) {
#line 133
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 133
		while (Cycle == (gl->start).cycle) {
#line 133
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 133
			if (Error != 0) {
#line 133
				break;
#line 133
			}
#line 133
		}
#line 133
		pthread_setcancelstate(Cancel, &Temp);
#line 133
	} else {
#line 133
		(gl->start).cycle = !(gl->start).cycle;
#line 133
		(gl->start).counter = 0;
#line 133
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 133
	}
#line 133
	pthread_mutex_unlock(&(gl->start).mutex);
#line 133
};

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 136
	struct timeval	FullTime;
#line 136

#line 136
	gettimeofday(&FullTime, NULL);
#line 136
	(gl->intraend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 136
};
            gl->intratime += gl->intraend - gl->interend;
        }

        TKIN=TKIN+gl->SUM[0]+gl->SUM[1]+gl->SUM[2];
        TVIR=TVIR-gl->VIR;

        /* CHECK if  PRINTING AND/OR SAVING IS TO BE DONE */

        if ( ((i % NPRINT) == 0) || ((NSAVE > 0) && ((i % NSAVE) == 0))) {

            /* if so, call poteng to compute potential energy.  Note
               that we are attributing all the time in poteng to intermolecular
               computation although some of it is intramolecular (see poteng.C) */

            if ((ProcID == 0) && (i >= 2)) {
                {
#line 152
	struct timeval	FullTime;
#line 152

#line 152
	gettimeofday(&FullTime, NULL);
#line 152
	(gl->interstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 152
};
            }

            POTENG(&gl->POTA,&gl->POTR,&gl->POTRF,ProcID);

            {
#line 157
	unsigned long	Error, Cycle;
#line 157
	long		Cancel, Temp;
#line 157

#line 157
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 157
	if (Error != 0) {
#line 157
		printf("Error while trying to get lock in barrier.\n");
#line 157
		exit(-1);
#line 157
	}
#line 157

#line 157
	Cycle = (gl->start).cycle;
#line 157
	if (++(gl->start).counter != (NumProcs)) {
#line 157
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 157
		while (Cycle == (gl->start).cycle) {
#line 157
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 157
			if (Error != 0) {
#line 157
				break;
#line 157
			}
#line 157
		}
#line 157
		pthread_setcancelstate(Cancel, &Temp);
#line 157
	} else {
#line 157
		(gl->start).cycle = !(gl->start).cycle;
#line 157
		(gl->start).counter = 0;
#line 157
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 157
	}
#line 157
	pthread_mutex_unlock(&(gl->start).mutex);
#line 157
};

            if ((ProcID == 0) && (i >= 2)) {
                {
#line 160
	struct timeval	FullTime;
#line 160

#line 160
	gettimeofday(&FullTime, NULL);
#line 160
	(gl->interend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 160
};
                gl->intertime += gl->interend - gl->interstart;
            }

            POTA=gl->POTA*FPOT;
            POTR=gl->POTR*FPOT;
            POTRF=gl->POTRF*FPOT;
            XVIR=TVIR*FPOT*0.50/TTMV;
            AVGT=TKIN*FKIN*TEMP*2.00/(3.00*TTMV);
            TEN=(gl->SUM[0]+gl->SUM[1]+gl->SUM[2])*FKIN;
            XTT=POTA+POTR+POTRF+TEN;

            /* if it is time to print output as well ... */
            if ((i % NPRINT) == 0 && ProcID == 0) {
                {pthread_mutex_lock(&(gl->IOLock));};
                fprintf(six,"     %5ld %14.5lf %12.5lf %12.5lf %12.5lf \n"
                        ,i,TEN,POTA,POTR,POTRF);
                fprintf(six," %16.3lf %16.5lf %16.5lf\n",XTT,AVGT,XVIR);
                fflush(six);
                {pthread_mutex_unlock(&(gl->IOLock));};
            }

        }

        {
#line 184
	unsigned long	Error, Cycle;
#line 184
	long		Cancel, Temp;
#line 184

#line 184
	Error = pthread_mutex_lock(&(gl->start).mutex);
#line 184
	if (Error != 0) {
#line 184
		printf("Error while trying to get lock in barrier.\n");
#line 184
		exit(-1);
#line 184
	}
#line 184

#line 184
	Cycle = (gl->start).cycle;
#line 184
	if (++(gl->start).counter != (NumProcs)) {
#line 184
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 184
		while (Cycle == (gl->start).cycle) {
#line 184
			Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
#line 184
			if (Error != 0) {
#line 184
				break;
#line 184
			}
#line 184
		}
#line 184
		pthread_setcancelstate(Cancel, &Temp);
#line 184
	} else {
#line 184
		(gl->start).cycle = !(gl->start).cycle;
#line 184
		(gl->start).counter = 0;
#line 184
		Error = pthread_cond_broadcast(&(gl->start).cv);
#line 184
	}
#line 184
	pthread_mutex_unlock(&(gl->start).mutex);
#line 184
};

        if ((ProcID == 0) && (i >= 2)) {
            {
#line 187
	struct timeval	FullTime;
#line 187

#line 187
	gettimeofday(&FullTime, NULL);
#line 187
	(gl->trackend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 187
};
            gl->tracktime += gl->trackend - gl->trackstart;
        }

    } /* for i */

    return(XTT);

} /* mdmain.c */
