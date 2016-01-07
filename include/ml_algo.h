/** @file ml_algo.h
 *  @brief Provide the base class for specific maching leanring algorithm.
 *
 *  This file contains all the necessary function support for specific machine learning algorithm.
 *
 *  @author Li Jiaying
 *  @bug 
 */
#ifndef _ML_ALGO_H_
#define _ML_ALGO_H_
#include <iostream>
#include "states.h"
#include "equation.h"

class MLalgo 
{
	public:
		MLalgo() {};

		/** @brief init training data method. This should be called before any training happens.
		 *
		 *  @param gsets The states array to store all the generated states information. 
		 *				 The size must be 4, and index -1 should be accessible
		 *  @param pre_positive_size This records the last positive size of states.
		 *							 And also set by callee to the new value
		 *							 Initially set to 0, as there is no elements in positive states.
		 *							 Calls afterwards should pass the value set by last call.
		 *  @param pre_negative_size This records the last negative size of states.
		 *							 And also set by callee to the new value
		 *							 Initially set to 0, as there is no elements in positive states.
		 *							 Calls afterwards should pass the value set by last call.
		 *	@return int 0 if no error
		 */
		virtual int makeTrainingSet(States* gsets, int& pre_positive_size, int&pre_negative_size) = 0;

		/** @brief The most important TRAIN method, which calls real training algorithm to do training
		 *
		 * @return int 0 if no error.
		 */
		virtual int train() = 0;

		/** @brief Calculate the predict precision of the training-model on the training set. 
		 *
		 * @return double Return precision we can get. Should be a value between 0 and 1.
		 */
		virtual double checkTrainingSet() = 0;

		/** @brief test on question state sets to see if there is an invalidation 
		 *
		 * The method will output the inforamtion if a question trace invalidate the training model
		 *
		 * @param qset is a reference type to question states. 
		 * @return int 0 if no error 
		 */
		virtual int checkQuestionSet(States& qset) = 0;

		/** @brief check whether the training is converged or not 
		 *
		 * current_training_equations ~= previous_trainig_equations ???
		 * 
		 * @param previous_equations contains all the equation we get from last trainig session. 
		 * @param equation_num is the number of equations get from last training session 
		 * @return int 0 if converged 
		 */
		virtual int converged(Equation* previous_equations, int equation_num) = 0;

		/** @brief output the current trainig result of a ML_Algo
		 *
		 * @param mla the ml_algo object to be output
		 * @return std::ostream
		 */
		friend std::ostream& operator << (std::ostream& out, const MLalgo& mla) { return mla._print(out); }

		/** @brief This is the function really called to output this object.
		 *		   We involve this as to support polymophism for operator <<
		 */
		virtual std::ostream& _print(std::ostream& out) const {
			out << "ML-algo: NULL";
			return out;
		};


		/** @brief This method returns the current problem size (the number of training states).
		 * 
		 * @return int the size of problem
		 */
		virtual int getProblemSize() = 0;


		/** @brief Round off the whole training model.(equations)
		 *
		 * @param equation_num set by callee to notify the number of equations we currently get
		 * @return Eqation Point the rounded off equations. 
		 *		   Remember to DELETE them after use by caller.
		 *		   Otherwise memory leak.
		 */
		virtual Equation* roundoff(int& equation_num) = 0;


		/** @brief Predict sample x against the whole training model.(equations)
		 *
		 * @param x contains the sample to be tested. 
		 * @return The label of prediction 
		 */
		virtual int predict(double* x) = 0;
	private:
};

#endif /* _ML_Algo_H */
