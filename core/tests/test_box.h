// -------------------------------------------------------------
//  Cubzh Core Unit Tests
//  test_blockChange.h
//  Created by Nino PLANE on October 25, 2022.
// -------------------------------------------------------------

#pragma once

#include "box.h"
#include "float3.h"
#include "int3.h"

void test_box_new(void) {
    Box* a = box_new();
    float3* limitCheck = float3_new(0.0f, 0.0f, 0.0f); 

    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(limitCheck);
    box_free(a);
}

void test_box_new_2(void) {
    Box* a = box_new_2(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    float3* limitCheck = float3_new(0.0f, 0.0f, 0.0f);

    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 1.0f, 1.0f, 1.0f);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(limitCheck);
    box_free(a);
}

void test_box_new_copy(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new_copy(a);
    float3* limitCheck = float3_new(3.0f, 5.0f, 2.0f);

    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 13.0f, 15.0f, 12.0f);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(limitCheck);
    box_free(a);
    box_free(b);
}

void test_box_set_bottom_center_position(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    float3* setcenter = float3_new(10.0f, 10.0f, 10.0f);
    float3* limitCheck = float3_new(5.0f, 10.0f, 5.0f);

    box_set_bottom_center_position(a, setcenter);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 15.0f, 20.0f, 15.0f);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(limitCheck);
    float3_free(setcenter);
    box_free(a);
}

void test_box_get_center(void) {
    Box* a = box_new_2(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f);
    Box* b = box_new_2(8.0f, 15.0f, 1.0f, 16.0f, 35.0f, 2.0f);
    float3* getcenter = float3_new(0.0f, 0.0f, 0.0f);
    float3* centerCheck = float3_new(5.0f, 5.0f, 5.0f);

    box_get_center(a, getcenter);
    TEST_CHECK(float3_isEqual(getcenter, centerCheck, EPSILON_0_0001_F) == true);
    box_get_center(b, getcenter);
    float3_set(centerCheck, 12.0f, 25.0f, 1.5f);
    TEST_CHECK(float3_isEqual(getcenter, centerCheck, EPSILON_0_0001_F) == true);

    float3_free(centerCheck);
    float3_free(getcenter);
    box_free(a);
    box_free(b);
}

void test_box_copy(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new_2(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    float3* limitCheck = float3_new(3.0f, 5.0f, 2.0f);

    box_copy(b, a);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 13.0f, 15.0f, 12.0f);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(limitCheck);
    box_free(a);
    box_free(b);
}

void test_box_collide(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new_2(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Box* c = box_new_2(2.0f, 4.0f, 1.0f, 14.0f, 16.0f, 13.0f);

    TEST_CHECK(box_collide(a, b) == false);
    TEST_CHECK(box_collide(c, b) == false);
    TEST_CHECK(box_collide(a, c) == true);

    TEST_CHECK(box_collide_epsilon(a, b, EPSILON_COLLISION) == false);
    TEST_CHECK(box_collide_epsilon(c, b, EPSILON_COLLISION) == false);
    TEST_CHECK(box_collide_epsilon(a, c, EPSILON_COLLISION) == true);

    box_free(a);
    box_free(b);
    box_free(c);
}

void test_box_contains(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    float3* pointCheck = float3_new(0.0f, 0.0f, 0.0f);

    TEST_CHECK(box_contains(a, pointCheck) == false);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == false);
    float3_set(pointCheck, 8.0f, 10.0f, 7.0f);
    TEST_CHECK(box_contains(a, pointCheck) == true);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == true);
    float3_set(pointCheck, 3.0f, 5.0f, 2.0f);
    TEST_CHECK(box_contains(a, pointCheck) == true);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == true);
    float3_set(pointCheck, 13.0f, 15.0f, 12.0f);
    TEST_CHECK(box_contains(a, pointCheck) == true);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == true);
    float3_set(pointCheck, 2.9f, 4.9f, 1.9f);
    TEST_CHECK(box_contains(a, pointCheck) == false);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == false);
    float3_set(pointCheck, 13.1f, 15.1f, 12.1f);
    TEST_CHECK(box_contains(a, pointCheck) == false);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == false);
    float3_set(pointCheck, 3.1f, 5.1f, 2.1f);
    TEST_CHECK(box_contains(a, pointCheck) == true);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == true);
    float3_set(pointCheck, 12.9f, 14.9f, 11.9f);
    TEST_CHECK(box_contains(a, pointCheck) == true);
    TEST_CHECK(box_contains_epsilon(a, pointCheck, EPSILON_COLLISION) == true);

    float3_free(pointCheck);
    box_free(a);
}

void test_box_set_broadphase_box(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new();
    float3* addToBpBox = float3_new(5.0f, 5.0f, 5.0f);
    float3* limitCheck = float3_new(3.0f, 5.0f, 2.0f);

    box_set_broadphase_box(a, addToBpBox, b);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 18.0f, 20.0f, 17.0f);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_set(addToBpBox, -1.0f, -3.0f, -2.0f);
    box_set_broadphase_box(a, addToBpBox, b);
    float3_set(limitCheck, 2.0f, 2.0f, 0.0f);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 13.0f, 15.0f, 12.0f);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(addToBpBox);
    float3_free(limitCheck);
    box_free(a);
    box_free(b);
}

void test_box_get_size(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new();
    float3 * boxSizeFloat = float3_new(0.0f, 0.0f, 0.0f);
    float3 * boxSizeFloatCheck = float3_new(10.0f, 10.0f, 10.0f);

    box_get_size_float(a, boxSizeFloat);
    TEST_CHECK(float3_isEqual(boxSizeFloat, boxSizeFloatCheck, EPSILON_0_0001_F) == true);
    box_get_size_float(b, boxSizeFloat);
    float3_set(boxSizeFloatCheck, 0.0f, 0.0f, 0.0f);
    TEST_CHECK(float3_isEqual(boxSizeFloat, boxSizeFloatCheck, EPSILON_0_0001_F) == true);

    int3* boxSizeInt = int3_new(0, 0, 0);
    int3* boxSizeIntCheck = int3_new(10, 10, 10);
    box_get_size_int(a, boxSizeInt);
    TEST_CHECK(boxSizeInt->x == boxSizeIntCheck->x);
    TEST_CHECK(boxSizeInt->y == boxSizeIntCheck->y);
    TEST_CHECK(boxSizeInt->z == boxSizeIntCheck->z);
    box_get_size_int(b, boxSizeInt);
    int3_set(boxSizeIntCheck, 0, 0, 0);
    TEST_CHECK(boxSizeInt->x == boxSizeIntCheck->x);
    TEST_CHECK(boxSizeInt->y == boxSizeIntCheck->y);
    TEST_CHECK(boxSizeInt->z == boxSizeIntCheck->z);

    float3_free(boxSizeFloat);
    float3_free(boxSizeFloatCheck);
    int3_free(boxSizeInt);
    int3_free(boxSizeIntCheck);
    box_free(a);
    box_free(b);
}

void test_box_is_empty(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new_2(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Box* c = box_new_2(-3.0f, -5.0f, -2.0f, -3.0f, -5.0f, -2.0f);

    TEST_CHECK(box_is_empty(a) == false);
    TEST_CHECK(box_is_empty(b) == true);
    TEST_CHECK(box_is_empty(c) == true);

    box_free(a);
    box_free(b);
    box_free(c);
}

void test_box_squarify(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 8.0f, 7.0f, 10.0f);
    Box* b = box_new_2(3.0f, 5.0f, 2.0f, 8.0f, 7.0f, 10.0f);
    float3* limitCheck = float3_new(3.0f, 5.0f, 2.0f);

    box_squarify(a, NoSquarify);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 8.0f, 7.0f, 10.0f);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == true);

    box_squarify(a, MinSquarify);
    float3_set(limitCheck, 3.0f, 5.0f, 3.5f);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 8.0f, 7.0f, 8.5f);
    TEST_CHECK(float3_isEqual(&a->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&a->max, limitCheck, EPSILON_0_0001_F) == true);

    box_squarify(b, MaxSquarify);
    float3_set(limitCheck, 1.5f, 5.0f, 2.0f);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 9.5f, 7.0f, 10.0f);
    TEST_CHECK(float3_isEqual(&b->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&b->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(limitCheck);
    box_free(a);
}

void test_box_op_merge(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new_2(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Box* result = box_new();
    float3* limitCheck = float3_new(0.0f, 0.0f, 0.0f);

    box_op_merge(a, b, result);
    TEST_CHECK(float3_isEqual(&result->min, limitCheck, EPSILON_0_0001_F) == true);
    TEST_CHECK(float3_isEqual(&result->max, limitCheck, EPSILON_0_0001_F) == false);
    float3_set(limitCheck, 13.0f, 15.0f, 12.0f);
    TEST_CHECK(float3_isEqual(&result->min, limitCheck, EPSILON_0_0001_F) == false);
    TEST_CHECK(float3_isEqual(&result->max, limitCheck, EPSILON_0_0001_F) == true);

    float3_free(limitCheck);
    box_free(a);
    box_free(b);
    box_free(result);
}

void test_box_get_volume(void) {
    Box* a = box_new_2(3.0f, 5.0f, 2.0f, 13.0f, 15.0f, 12.0f);
    Box* b = box_new();
    Box* c = box_new_2(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    float result = box_get_volume(a);
    TEST_CHECK(result == 1000.0f);
    result = box_get_volume(b);
    TEST_CHECK(result == 0.0f);
    result = box_get_volume(c);
    TEST_CHECK(result == 1.0f);

    box_free(a);
    box_free(b);
    box_free(c);
}