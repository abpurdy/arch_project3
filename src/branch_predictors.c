//
// This file contains all of the implementations of the branch predictor
// constructors from the branch_predictors.h file.
//
// It also contains stubs of all of the functions that are added to each
// branch_predictor struct at construction time.
//
// Feel free to create any additional structs or global variables that you
// want.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// branch_predictors.h file for further context on what each function is for.
// ============================================================================
//

#include "branch_predictors.h"

// ANT Branch Predictor
// ============================================================================

enum branch_direction ant_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    return NOT_TAKEN;  

}

void ant_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
}

void ant_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup if necessary
}

struct branch_predictor *ant_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ant_bp = calloc(1, sizeof(struct branch_predictor));
    ant_bp->cleanup = &ant_branch_predictor_cleanup;
    ant_bp->predict = &ant_branch_predictor_predict;
    ant_bp->handle_result = &ant_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor

    return ant_bp;
}

// AT Branch Predictor
// ============================================================================

enum branch_direction at_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                  uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    return TAKEN;
}

void at_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                       enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
}

void at_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // leanup if necessary
}

struct branch_predictor *at_branch_predictor_new(uint32_t num_branches,
                                                 struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *at_bp = calloc(1, sizeof(struct branch_predictor));
    at_bp->cleanup = &at_branch_predictor_cleanup;
    at_bp->predict = &at_branch_predictor_predict;
    at_bp->handle_result = &at_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor

    return at_bp;
}

// BTFNT Branch Predictor
// ============================================================================
struct btfnt_data{
	uint32_t prev_address;
};

enum branch_direction btfnt_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                     uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    if(address < ((struct btfnt_data*) branch_predictor->data)->prev_address)
	    return TAKEN;
    return NOT_TAKEN;
}

void btfnt_branch_predictor_handle_result(struct branch_predictor *branch_predictor,
                                          uint32_t address, enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    ((struct btfnt_data*) branch_predictor->data)->prev_address = address;
}

void btfnt_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup if necessary
    free(((struct btfnt_data*) branch_predictor->data));
}

struct branch_predictor *btfnt_branch_predictor_new(uint32_t num_branches,
                                                    struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *btfnt_bp = calloc(1, sizeof(struct branch_predictor));
    btfnt_bp->cleanup = &btfnt_branch_predictor_cleanup;
    btfnt_bp->predict = &btfnt_branch_predictor_predict;
    btfnt_bp->handle_result = &btfnt_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    struct btfnt_data *new_data = malloc(sizeof(struct btfnt_data));
    new_data->prev_address = -1;
    btfnt_bp->data = new_data;

    return btfnt_bp;
}

// LTG Branch Predictor
// ============================================================================
struct ltg_data{
	uint32_t ghr;
	enum branch_direction *pht;
};

enum branch_direction ltg_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    return ((struct ltg_data*) branch_predictor->data)->pht[((struct ltg_data*) branch_predictor->data)->ghr];
}

void ltg_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t ghr = ((struct ltg_data*) branch_predictor->data)->ghr;
    ((struct ltg_data*) branch_predictor->data)->pht[ghr] = branch_direction;
    ghr = ((ghr << 28) >> 27);
    if(branch_direction == TAKEN)
	    ghr++;
    ((struct ltg_data*) branch_predictor->data)->ghr = ghr;
}

void ltg_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup if necessary
    free(((struct ltg_data*) branch_predictor->data)->pht);
    free(((struct ltg_data*) branch_predictor->data));
}

struct branch_predictor *ltg_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ltg_bp = calloc(1, sizeof(struct branch_predictor));
    ltg_bp->cleanup = &ltg_branch_predictor_cleanup;
    ltg_bp->predict = &ltg_branch_predictor_predict;
    ltg_bp->handle_result = &ltg_branch_predictor_handle_result;

    // allocate sto rage for any data necessary for this branch predictor
    struct ltg_data *new_data = malloc(sizeof(struct ltg_data));
    new_data->pht = malloc(32*sizeof(enum branch_direction));
    for(int i = 0; i < 32; i++){
	    new_data->pht[i] = NOT_TAKEN;
    }
    new_data->ghr = 0;
    ltg_bp->data = new_data;

    return ltg_bp;
}

// LTL Branch Predictor
// ============================================================================
struct ltl_data{
	uint32_t *lhr;
	enum branch_direction *pht;
};

enum branch_direction ltl_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.i
    uint32_t lhr_index = ((address << 28) >> 28);
    return ((struct ltl_data*) branch_predictor->data)->pht[16*lhr_index + ((struct ltl_data*) branch_predictor->data)->lhr[lhr_index]];

}

void ltl_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t lhr_index = ((address << 28) >> 28);
    uint32_t lhr= ((struct ltl_data*) branch_predictor->data)->lhr[lhr_index];
    ((struct ltl_data*) branch_predictor->data)->pht[16*lhr_index + lhr] = branch_direction;

    lhr = ((lhr << 29) >> 28);
    if(branch_direction == TAKEN)
	    lhr++;
    ((struct ltl_data*) branch_predictor->data)->lhr[lhr_index] = lhr;
}

void ltl_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup if necessary
    free(((struct ltl_data*) branch_predictor->data)->lhr);
    free(((struct ltl_data*) branch_predictor->data)->pht);
    free(((struct ltl_data*) branch_predictor->data));
}

struct branch_predictor *ltl_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ltl_bp = calloc(1, sizeof(struct branch_predictor));
    ltl_bp->cleanup = &ltl_branch_predictor_cleanup;
    ltl_bp->predict = &ltl_branch_predictor_predict;
    ltl_bp->handle_result = &ltl_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    struct ltl_data *new_data = malloc(sizeof(struct ltl_data));
    new_data->pht = malloc(16*16*sizeof(enum branch_direction));
    for(int i = 0; i < 256; i++){
	    new_data->pht[i] = NOT_TAKEN;
    }
    new_data->lhr = malloc(16*sizeof(uint32_t));
    for(int i = 0; i < 16; i++){
	    new_data->lhr[i] = 0;
    }
    ltl_bp->data = new_data;

    return ltl_bp;
}

// 2BG Branch Predictor
// ============================================================================
struct tbg_data{
	uint32_t ghr;
	uint32_t *pht;
};

enum branch_direction tbg_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    if(((struct tbg_data*) branch_predictor->data)->pht[((struct tbg_data*) branch_predictor->data)->ghr] > 1)
	    return TAKEN;
    return NOT_TAKEN;
}

void tbg_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t ghr = ((struct tbg_data*) branch_predictor->data)->ghr;
    uint32_t pht = ((struct tbg_data*) branch_predictor->data)->pht[ghr];
    if(branch_direction == TAKEN && pht < 3){
	    ((struct tbg_data*) branch_predictor->data)->pht[ghr]++;
    }else if(branch_direction == NOT_TAKEN && pht > 0){
	    ((struct tbg_data*) branch_predictor->data)->pht[ghr]--;
    }

    ghr = ((ghr << 28) >> 27);
    if(branch_direction == TAKEN)
	    ghr++;
    ((struct tbg_data*) branch_predictor->data)->ghr = ghr;

}

void tbg_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup if necessary
    free(((struct tbg_data*) branch_predictor->data)->pht);
    free(((struct tbg_data*) branch_predictor->data));
}

struct branch_predictor *tbg_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *tbg_bp = calloc(1, sizeof(struct branch_predictor));
    tbg_bp->cleanup = &tbg_branch_predictor_cleanup;
    tbg_bp->predict = &tbg_branch_predictor_predict;
    tbg_bp->handle_result = &tbg_branch_predictor_handle_result;

    //allocate storage for any data necessary for this branch predictor
    struct tbg_data *new_data = malloc(sizeof(struct tbg_data));
    new_data->pht = malloc(32*sizeof(uint32_t));
    for(int i = 0; i < 32; i++){
	    new_data->pht[i] = 0;
    }
    new_data->ghr = 0;
    tbg_bp->data = new_data;

    return tbg_bp;
}

// 2BL Branch Predictor
// ============================================================================
struct tbl_data{
	uint32_t *lhr;
	uint32_t *pht;
};

enum branch_direction tbl_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.
    uint32_t lhr_index = ((address << 28) >> 28);
    if(((struct ltl_data*) branch_predictor->data)->pht[16*lhr_index + ((struct ltl_data*) branch_predictor->data)->lhr[lhr_index]] > 1)
	    return TAKEN;
    return NOT_TAKEN;
}

void tbl_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t lhr_index = ((address << 28) >> 28);
    uint32_t lhr = ((struct tbl_data*) branch_predictor->data)->lhr[lhr_index];
    uint32_t pht = ((struct tbl_data*) branch_predictor->data)->pht[16*lhr_index + lhr];
    if(branch_direction == TAKEN && pht < 3){
	    ((struct tbl_data*) branch_predictor->data)->pht[16*lhr_index + lhr]++;
    }else if(branch_direction == NOT_TAKEN && pht > 0){
	    ((struct tbl_data*) branch_predictor->data)->pht[16*lhr_index + lhr]--;
    }

    lhr = ((lhr << 29) >> 28);
    if(branch_direction == TAKEN)
	    lhr++;
    ((struct tbl_data*) branch_predictor->data)->lhr[lhr_index] = lhr;
}

void tbl_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup if necessary
    free(((struct tbl_data*) branch_predictor->data)->lhr);
    free(((struct tbl_data*) branch_predictor->data)->pht);
    free(((struct tbl_data*) branch_predictor->data));

}

struct branch_predictor *tbl_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *tbl_bp = calloc(1, sizeof(struct branch_predictor));
    tbl_bp->cleanup = &tbl_branch_predictor_cleanup;
    tbl_bp->predict = &tbl_branch_predictor_predict;
    tbl_bp->handle_result = &tbl_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    struct tbl_data *new_data = malloc(sizeof(struct tbl_data));
    new_data->pht = malloc(16*16*sizeof(uint32_t));
    for(int i = 0; i < 256; i++){
	    new_data->pht[i] = 0;
    }
    new_data->lhr = malloc(16*sizeof(uint32_t));
    for(int i = 0; i < 16; i++){
	    new_data->lhr[i] = 0;
    }
    tbl_bp->data = new_data;

    return tbl_bp;
}
