#ifndef _LIBSVM_H
#define _LIBSVM_H

#define LIBSVM_VERSION 310


/* extensions to svm.h headers  to support open set recognition based on

   W. J. Scheirer, A.  Rocha, A. Sapkota, T. E. Boult, "Toward Open Set Recognition," IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 35, no. 7, pp. 1757-1772, July, 2013    

@article{10.1109/TPAMI.2012.256,
author = {W. J. Scheirer and A. Rocha and A. Sapkota and T. E. Boult},
title = {Toward Open Set Recognition},
journal ={IEEE Transactions on Pattern Analysis and Machine Intelligence},
volume = {35},
number = {7},
issn = {0162-8828},
year = {2013},
pages = {1757-1772},
doi = {http://doi.ieeecomputersociety.org/10.1109/TPAMI.2012.256},
}


If you use any of the open set functions please cite appropriately.

There are also extensions using libMR which will be described in other
publications and should also cite based on libMR licensing if that is used as well.


These open set extensions to libSVM are subject to the following

Copyright (c) 2010-2013  Regents of the University of Colorado and Securics Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following 3 conditions:

1) The above copyright notice and this permission notice shall be included in all
source code copies or substantial portions of the Software.

2) All documentation and/or advertising materials mentioning features or use of
this software must display the following acknowledgment:

      This product includes software developed in part at
      the University of Colorado Colorado Springs and Securics Inc.

3) Neither the name of Regents of the University of Colorado  and Securics Inc.  nor
 the names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


*/ 


#ifdef USELIBMR
#define USEPIESVM
#include "MetaRecognition.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern int libsvm_version;

struct svm_node
{
	int index;
	double value;
};

struct svm_problem
{
  int l;
  double *y;
  struct svm_node **x;
  int nr_classes;
  int *labels;
};


  typedef enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR, OPENSET_OC, OPENSET_PAIR, OPENSET_BIN, OPEN_PIESVM, PAIR_PIESVM, ONE_VS_REST_PIESVM, ONE_PIESVM } svm_type_t;	/* svm_type */
  typedef enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED } kernel_t; /* kernel_type */

  typedef enum {OPT_PRECISION, OPT_RECALL,  OPT_FMEASURE,  OPT_HINGE, OPT_BALANCEDRISK}  openset_optimization_t;



struct svm_parameter
{
	int svm_type;
	int kernel_type;
	int do_open;	/* do we want to do open-set expansion of base kernel */
	int degree;	/* for poly */
	double gamma;	/* for poly/rbf/sigmoid */
	double coef0;	/* for poly/sigmoid */
	/* these are for training only */
	double cache_size; /* in MB */
	double eps;	/* stopping criteria */
	double C;	/* for C_SVC, EPSILON_SVR and NU_SVR */
	int nr_weight;		/* for C_SVC */
	int nr_fold;		/* for cross-validation in training */
	int cross_validation;		/* for cross-validation */
	int *weight_label;	/* for C_SVC */
	double* weight;		/* for C_SVC */
	double nu;	/* for NU_SVC, ONE_CLASS, and NU_SVR */
	double p;	/* for EPSILON_SVR */
	int shrinking;	/* use the shrinking heuristics */
	int probability; /* do probability estimates */
	bool neg_labels; /* do we consider negative class labels (like -1) as a label for openset, or just negative.. default is false */
	bool exaustive_open; /* do we do exaustive optimization for openset.. default is false */
        openset_optimization_t optimize; /* choice of what to optimize */ 
        double beta; /* for use in f-measure optimization */ 
        double near_preasure, far_preasure; /* for openset risk preasures */
        double openset_min_probability; /* for PIESVM openset, what is minimum probability to consider positive */
        FILE* vfile; /* for logging verbose stuff during debugging */ 
        int  rejectedID; /* id for rejected classes (-99999 is the default) */ 
};

//
// svm_model
// 
struct svm_model
{
	struct svm_parameter param;	/* parameter */
	int nr_class;		/* number of classes, = 2 in regression/one class svm */
	int l;			/* total #SV */
	struct svm_node **SV;		/* SVs (SV[l]) */
	double **sv_coef;	/* coefficients for SVs in decision functions (sv_coef[k-1][l]) */
	double *rho;		/* constants in decision functions (rho[k*(k-1)/2]) */
	double *probA;		/* pariwise probability information */
	double *probB;
        int openset_dim;        /* dimension of data for 1-vs-set models,  if open_set, wsvm or open_bset (5,6,7) then openset_dim=k, if open_pair then its k*(k-1)/2*/
        double *alpha, *omega;  /* planes offsets for 1-vs-set   alpha[openset_dim], omega[openset_dim] */
        double *wbltrans,*wblshape,*wblscale;        /* weibul parms for wsvm   all of dimension [openset_dim] */
#ifdef USEPIESVM
		MetaRecognition *MRpos_one_vs_all, *MRcomp_one_vs_all;   //MetaRecognition Objects for positive (inclass) and complement classifiers for 1-vs-all
		MetaRecognition *MRpos_binary_pairs, *MRcomp_binary_pairs;   //MetaRecognition Objects for positive (inclass) and complement classifiers for pair-wise binary
		MetaRecognition *MRpos_one_class;	//MetaRecognition Objects for positive (inclass) one-class 
#endif


	/* for classification only */

	int *label;		/* label of each class (label[k]) */
	int *nSV;		/* number of SVs for each class (nSV[k]) */
				/* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
	/* XXX */
	int free_sv;		/* 1 if svm_model is created by svm_load_model and needs to free its memory*/
				/* 0 if svm_model is created by svm_train */
};

struct svm_model *svm_train(const struct svm_problem *prob, const struct svm_parameter *param);
void svm_cross_validation(const struct svm_problem *prob, const struct svm_parameter *param, int nr_fold, double *target);

int svm_save_model(const char *model_file_name, const struct svm_model *model);
struct svm_model *svm_load_model(const char *model_file_name);

int svm_get_svm_type(const struct svm_model *model);
int svm_get_nr_class(const struct svm_model *model);
void svm_get_labels(const struct svm_model *model, int *label);
double svm_get_svr_probability(const struct svm_model *model);
double svm_predict_values(const struct svm_model *model, const struct svm_node *x, double* dec_values);
double svm_predict_values_extended(const struct svm_model *model, const struct svm_node *x, 
								   double*& dec_values, double **&scores, int*& vote);
double svm_predict(const struct svm_model *model, const struct svm_node *x);
double svm_predict_extended(const struct svm_model *model, const struct svm_node *x,
							double **&scores, int *&vote);
double svm_predict_probability(const struct svm_model *model, const struct svm_node *x, double* prob_estimates);

void svm_free_model_content(struct svm_model *model_ptr);
void svm_free_and_destroy_model(struct svm_model **model_ptr_ptr);
void svm_destroy_param(struct svm_parameter *param);

const char *svm_check_parameter(const struct svm_problem *prob, const struct svm_parameter *param);
int svm_check_probability_model(const struct svm_model *model);

void svm_set_print_string_function(void (*print_func)(const char *));

  typedef  unsigned long ulong;
#ifdef __cplusplus
}
#endif

#endif /* _LIBSVM_H */
