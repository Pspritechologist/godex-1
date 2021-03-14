#ifndef TEST_ECS_QUERY_H
#define TEST_ECS_QUERY_H

#include "tests/test_macros.h"

#include "../components/dynamic_component.h"
#include "../ecs.h"
#include "../iterators/dynamic_query.h"
#include "../modules/godot/components/transform_component.h"
#include "../storage/batch_storage.h"
#include "../world/world.h"

struct TagQueryTestComponent {
	COMPONENT(TagQueryTestComponent, DenseVectorStorage)
	static void _bind_methods() {}
};

struct TagA {
	COMPONENT(TagA, DenseVectorStorage)
};

struct TagB {
	COMPONENT(TagB, DenseVectorStorage)
};

struct TagC {
	COMPONENT(TagC, DenseVectorStorage)
};

struct TestFixedSizeEvent {
	COMPONENT_BATCH(TestFixedSizeEvent, DenseVector, 2)
	static void _bind_methods() {}

	int number = 0;

	TestFixedSizeEvent(int num) :
			number(num) {}
};

struct TestEvent {
	COMPONENT_BATCH(TestEvent, DenseVector, -1) // -1 make the storage dynamic.
	static void _bind_methods() {}

	int number = 0;

	TestEvent(int num) :
			number(num) {}
};

namespace godex_tests {

TEST_CASE("[Modules][ECS] Test fetch element type using fetch_element_type.") {
	ECS::register_component<TagA>();
	ECS::register_component<TagB>();
	ECS::register_component<TagC>();

	// Make sure the `fetch_element_type` is able to fetch always the correct
	// type, basing on the given `Search` (the first number in the template
	// list) and the following parameters.
	// Check the doc on the `fetch_element` (in query.h) to know more about it.

	// Test fetch element type
	{
		int xx = 30;
		int jj = 10;
		float gg = 20;
		bool bb = 30;
		fetch_element_type<0, 0, int, Any<int, float>, bool> ptr_xx = &xx;
		fetch_element_type<1, 0, int, Any<int, float>, bool> ptr_jj = &jj;
		fetch_element_type<2, 0, int, Any<int, float>, bool> ptr_gg = &gg;
		fetch_element_type<3, 0, int, Any<int, float>, bool> ptr_bb = &bb;
		CHECK(ptr_xx == &xx);
		CHECK(ptr_jj == &jj);
		CHECK(ptr_gg == &gg);
		CHECK(ptr_bb == &bb);
	}

	// Test fetch element type with filters.
	{
		int xx = 30;
		int jj = 10;
		float gg = 20;
		bool bb = 30;
		EntityID ee;
		fetch_element_type<0, 0, Without<int>, Any<int, Changed<float>>, Maybe<bool>, EntityID> ptr_xx = &xx;
		fetch_element_type<1, 0, Without<int>, Any<int, Changed<float>>, Maybe<bool>, EntityID> ptr_jj = &jj;
		fetch_element_type<2, 0, Without<int>, Any<int, Changed<float>>, Maybe<bool>, EntityID> ptr_gg = &gg;
		fetch_element_type<3, 0, Without<int>, Any<int, Changed<float>>, Maybe<bool>, EntityID> ptr_bb = &bb;
		fetch_element_type<4, 0, Without<int>, Any<int, Changed<float>>, Maybe<bool>, EntityID> entity = ee;
		CHECK(ptr_xx == &xx);
		CHECK(ptr_jj == &jj);
		CHECK(ptr_gg == &gg);
		CHECK(ptr_bb == &bb);
		CHECK(entity == ee);
	}

	// Test fetch element type with filters batch deep.
	{
		int xx = 30;
		int jj = 10;
		float gg = 20;
		bool bb = 30;
		EntityID ee;
		fetch_element_type<0, 0, Without<int>, Any<int, Batch<Changed<float>>>, Maybe<bool>, EntityID> ptr_xx = &xx;
		fetch_element_type<1, 0, Without<int>, Any<int, Batch<Changed<float>>>, Maybe<bool>, EntityID> ptr_jj = &jj;
		fetch_element_type<2, 0, Without<int>, Any<int, Batch<Changed<float>>>, Maybe<bool>, EntityID> ptr_gg(&gg, 1);
		fetch_element_type<3, 0, Without<int>, Any<int, Batch<Changed<float>>>, Maybe<bool>, EntityID> ptr_bb = &bb;
		fetch_element_type<4, 0, Without<int>, Any<int, Batch<Changed<float>>>, Maybe<bool>, EntityID> entity = ee;
		CHECK(ptr_xx == &xx);
		CHECK(ptr_jj == &jj);
		CHECK(ptr_gg.is_empty() == false);
		CHECK(ptr_gg.get_size() == 1);
		CHECK(ptr_gg[0] == &gg);
		CHECK(ptr_bb == &bb);
		CHECK(entity == ee);
	}

	// Test fetch element type with filters Without & Maybe deep.
	{
		int xx = 30;
		int jj = 10;
		float gg = 20;
		bool bb = 30;
		EntityID ee;
		fetch_element_type<0, 0, Without<int>, Any<Without<Changed<int>>, Batch<Maybe<Changed<float>>>>, Maybe<bool>, EntityID> ptr_xx = &xx;
		fetch_element_type<1, 0, Without<int>, Any<Without<Changed<int>>, Batch<Maybe<Changed<float>>>>, Maybe<bool>, EntityID> ptr_jj = &jj;
		fetch_element_type<2, 0, Without<int>, Any<Without<Changed<int>>, Batch<Maybe<Changed<float>>>>, Maybe<bool>, EntityID> batch_gg(&gg, 1);
		fetch_element_type<3, 0, Without<int>, Any<Without<Changed<int>>, Batch<Maybe<Changed<float>>>>, Maybe<bool>, EntityID> ptr_bb = &bb;
		fetch_element_type<4, 0, Without<int>, Any<Without<Changed<int>>, Batch<Maybe<Changed<float>>>>, Maybe<bool>, EntityID> entity = ee;
		CHECK(ptr_xx == &xx);
		CHECK(ptr_jj == &jj);
		CHECK(batch_gg.is_empty() == false);
		CHECK(batch_gg.get_size() == 1);
		CHECK(batch_gg[0] == &gg);
		CHECK(ptr_bb == &bb);
		CHECK(entity == ee);
	}

	// Test fetch element type with filters Any and Join
	{
		int xx = 30;
		TagA gg;
		bool bb = 30;
		EntityID ee;
		fetch_element_type<0, 0, Without<int>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>, Maybe<bool>, EntityID> ptr_xx = &xx;
		fetch_element_type<1, 0, Without<int>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>, Maybe<bool>, EntityID> join(&gg, TagA::get_component_id(), false);
		fetch_element_type<2, 0, Without<int>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>, Maybe<bool>, EntityID> ptr_bb = &bb;
		fetch_element_type<3, 0, Without<int>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>, Maybe<bool>, EntityID> entity = ee;
		CHECK(ptr_xx == &xx);
		CHECK(join.is_null() == false);
		CHECK(join.is<TagA>());
		CHECK(join.as<TagA>() == &gg);
		CHECK(ptr_bb == &bb);
		CHECK(entity == ee);
	}

	// Test fetch element type with filters Any and Join deep
	{
		int xx = 30;
		TagA gg;
		bool bb = 30;
		EntityID ee;
		fetch_element_type<0, 0, Any<Without<Changed<int>>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>>, Maybe<bool>, EntityID> ptr_xx = &xx;
		fetch_element_type<1, 0, Any<Without<Changed<int>>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>>, Maybe<bool>, EntityID> join(&gg, TagA::get_component_id(), false);
		fetch_element_type<2, 0, Any<Without<Changed<int>>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>>, Maybe<bool>, EntityID> ptr_bb = &bb;
		fetch_element_type<3, 0, Any<Without<Changed<int>>, Join<Any<Without<Changed<TagA>>, Batch<Maybe<Changed<TagB>>>>>>, Maybe<bool>, EntityID> entity = ee;
		CHECK(ptr_xx == &xx);
		CHECK(join.is_null() == false);
		CHECK(join.is<TagA>());
		CHECK(join.as<TagA>() == &gg);
		CHECK(ptr_bb == &bb);
		CHECK(entity == ee);
	}
}

TEST_CASE("[Modules][ECS] Test QueryResultTuple: packing and unpaking following the Query rules.") {
	TagA a;
	TagB b;
	TagC c;

	// Test basic types get:
	// Make sure it's possible to compose a tuple and the stored data
	// can be correctly retrieved.
	{
		QueryResultTuple<TagA, TagB> tuple;

		static_assert(tuple.SIZE == 2);

		set<0>(tuple, &a);
		set<1>(tuple, &b);

		{
			TagA *ptr_a = get<0>(tuple);
			TagB *ptr_b = get<1>(tuple);
			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
		}

		{
			auto [ptr_a, ptr_b] = tuple;

			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
		}
	}

	// Test advanced nesting:
	// Make sure deep nesting is correctly flattened: so all the variables are
	// all in 1 dimention and can be easily accessed with just 1 structured
	// bindings as below.
	{
		QueryResultTuple<Any<TagA, TagB>, TagC> tuple;

		static_assert(tuple.SIZE == 3);

		set<0>(tuple, &a);
		set<1>(tuple, &b);
		set<2>(tuple, &c);

		{
			TagC *ptr_c = get<2>(tuple);
			TagA *ptr_a = get<0>(tuple);
			TagB *ptr_b = get<1>(tuple);

			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
			CHECK(ptr_c == &c);
		}

		{
			auto [ptr_a, ptr_b, ptr_c] = tuple;

			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
			CHECK(ptr_c == &c);
		}
	}

	// Test late initialization.
	{
		TransformComponent transf;

		QueryResultTuple<TransformComponent, Any<TagA, TagB>, TagC> tuple;

		static_assert(tuple.SIZE == 4);

		set<0>(tuple, &transf);
		set<1>(tuple, &a);
		set<2>(tuple, &b);
		set<3>(tuple, &c);

		{
			TransformComponent *transform_ptr = get<0>(tuple);
			TagA *ptr_a = get<1>(tuple);
			TagB *ptr_b = get<2>(tuple);
			TagC *ptr_c = get<3>(tuple);

			CHECK(transform_ptr == &transf);
			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
			CHECK(ptr_c == &c);
		}

		transf.transform.origin.x = -50;

		{
			auto [transform_ptr, ptr_a, ptr_b, ptr_c] = tuple;

			CHECK(transform_ptr == &transf);
			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
			CHECK(ptr_c == &c);

			CHECK(ABS(transform_ptr->transform.origin.x - transf.transform.origin.x) <= CMP_EPSILON);
		}
	}

	// Test other filters
	{
		QueryResultTuple<Without<TagA>, Maybe<TagB>, Changed<TagC>> tuple;

		set<0>(tuple, &a);
		set<1>(tuple, &b);
		set<2>(tuple, &c);

		static_assert(tuple.SIZE == 3);

		{
			TagA *ptr_a = get<0>(tuple);
			TagB *ptr_b = get<1>(tuple);
			TagC *ptr_c = get<2>(tuple);

			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
			CHECK(ptr_c == &c);
		}

		{
			auto [ptr_a, ptr_b, ptr_c] = tuple;

			CHECK(ptr_a == &a);
			CHECK(ptr_b == &b);
			CHECK(ptr_c == &c);
		}
	}

	// Test Filters deep
	{
		QueryResultTuple<Maybe<Changed<TagC>>> tuple;

		set<0>(tuple, &c);

		TagC *ptr_c = get<0>(tuple);
		CHECK(ptr_c == &c);
	}

	// Test `Batch` filter.
	{
		QueryResultTuple<Batch<TagA>, Maybe<const TagB>> tuple;

		set<0>(tuple, Batch(&a, 1));
		set<1>(tuple, &b);

		static_assert(tuple.SIZE == 2);

		{
			Batch<TagA *> batch_a = get<0>(tuple);
			const TagB *ptr_b = get<1>(tuple);

			CHECK(batch_a.is_empty() == false);
			CHECK(batch_a[0] == &a);
			CHECK(ptr_b == &b);
		}

		{
			auto [batch_a, ptr_b] = tuple;

			CHECK(batch_a.is_empty() == false);
			CHECK(batch_a[0] == &a);
			CHECK(ptr_b == &b);
		}
	}

	// Test `EntityID` filter.
	{
		QueryResultTuple<Batch<TagA>, EntityID, Maybe<const TagB>> tuple;

		static_assert(tuple.SIZE == 3);

		set<0>(tuple, Batch(&a, 1));
		set<1>(tuple, EntityID(2));
		set<2>(tuple, &b);

		{
			Batch<TagA *> batch_a = get<0>(tuple);
			EntityID entity = get<1>(tuple);
			const TagB *ptr_b = get<2>(tuple);

			CHECK(batch_a.is_empty() == false);
			CHECK(batch_a[0] == &a);
			CHECK(entity == EntityID(2));
			CHECK(ptr_b == &b);
		}

		{
			auto [batch_a, entity, ptr_b] = tuple;

			CHECK(batch_a.is_empty() == false);
			CHECK(batch_a[0] == &a);
			CHECK(entity == EntityID(2));
			CHECK(ptr_b == &b);
		}

		// Try set.

		TagA another_tag_a;
		set<0>(tuple, Batch(&another_tag_a, 1));
		set<1>(tuple, EntityID(4));

		{
			auto [batch_a, entity, ptr_b] = tuple;

			CHECK(batch_a.is_empty() == false);
			CHECK(batch_a[0] == &another_tag_a);
			CHECK(entity == EntityID(4));
			CHECK(ptr_b == &b);
		}
	}

	// Test `Join` filter.
	{
		QueryResultTuple<Join<Any<TagA, Changed<TagB>>>, EntityID> tuple;

		static_assert(tuple.SIZE == 2);

		set<0>(tuple, JoinData(&a, TagA::get_component_id(), false));
		set<1>(tuple, EntityID(5));

		{
			JoinData join = get<0>(tuple);
			EntityID entity = get<1>(tuple);

			CHECK(join.is<TagA>());
			CHECK(join.as<TagA>() == &a);
			CHECK(entity == EntityID(5));
		}

		{
			auto [join, entity] = tuple;

			CHECK(join.is<TagA>());
			CHECK(join.as<TagA>() == &a);
			CHECK(entity == EntityID(5));
		}

		// Try set.

		TagA another_tag_a;
		set<0>(tuple, JoinData(&another_tag_a, TagA::get_component_id(), false));
		set<1>(tuple, EntityID(10));

		{
			auto [join, entity] = tuple;

			CHECK(join.is<TagA>());
			CHECK(join.as<TagA>() == &another_tag_a);
			CHECK(entity == EntityID(10));
		}
	}

	// Test deep all filters.
	{
		QueryResultTuple<EntityID, Any<Maybe<Changed<TagC>>, Batch<Maybe<Changed<TagB>>>>, Join<Any<Without<TagA>, Changed<TagB>>>> tuple;

		static_assert(tuple.SIZE == 4);

		set<0>(tuple, EntityID(10));
		set<1>(tuple, &c);
		set<2>(tuple, Batch(&b, 1));
		set<3>(tuple, JoinData(&b, TagB::get_component_id(), false));

		{
			EntityID entity = get<0>(tuple);
			TagC *ptr_c = get<1>(tuple);
			Batch<TagB *> batch_b = get<2>(tuple);
			JoinData join = get<3>(tuple);

			CHECK(entity == EntityID(10));
			CHECK(ptr_c == &c);
			CHECK(batch_b.is_empty() == false);
			CHECK(batch_b[0] == &b);
			CHECK(join.is<TagB>());
			CHECK(join.as<TagB>() == &b);
		}

		{
			auto [entity, ptr_c, batch_b, join] = tuple;

			CHECK(entity == EntityID(10));
			CHECK(ptr_c == &c);
			CHECK(batch_b.is_empty() == false);
			CHECK(batch_b[0] == &b);
			CHECK(join.is<TagB>());
			CHECK(join.as<TagB>() == &b);
		}
	}

	// Test multiple nested Any filters.
	{
		QueryResultTuple<
				EntityID,
				Any<
						Maybe<Changed<TagC>>,
						Batch<Maybe<Changed<TagB>>>,
						Join<Any<Without<TagA>, Changed<TagB>>>,
						Any<
								Without<Changed<TagA>>,
								Changed<TagB>>>,
				EntityID,
				TransformComponent>
				tuple;

		static_assert(tuple.SIZE == 8);

		TagA another_tag_a;
		TagB another_tag_b;
		TransformComponent transf(Transform(Basis(), Vector3(1, 0, 0)));

		set<0>(tuple, EntityID(10));
		set<1>(tuple, &c);
		set<2>(tuple, Batch(&b, 1));
		set<3>(tuple, JoinData(&a, TagA::get_component_id(), false));
		set<4>(tuple, &another_tag_a);
		set<5>(tuple, &another_tag_b);
		set<6>(tuple, EntityID(100));
		set<7>(tuple, &transf);

		// Test GET
		{
			EntityID entity = get<0>(tuple);
			TagC *ptr_c = get<1>(tuple);
			Batch<TagB *> batch = get<2>(tuple);
			JoinData join = get<3>(tuple);
			TagA *ptr_another_a = get<4>(tuple);
			TagB *ptr_another_b = get<5>(tuple);
			EntityID another_entity = get<6>(tuple);
			TransformComponent *ptr_transf = get<7>(tuple);

			CHECK(entity == EntityID(10));
			CHECK(ptr_c == &c);
			CHECK(batch.is_empty() == false);
			CHECK(batch[0] == &b);
			CHECK(join.is<TagA>());
			CHECK(join.as<TagA>() == &a);
			CHECK(ptr_another_a == &another_tag_a);
			CHECK(ptr_another_b == &another_tag_b);
			CHECK(another_entity == EntityID(100));
			CHECK(ptr_transf == &transf);
		}

		// Test Structured bindings.
		{
			auto [entity, ptr_c, batch, join, ptr_another_a, ptr_another_b, another_entity, ptr_transf] = tuple;

			CHECK(entity == EntityID(10));
			CHECK(ptr_c == &c);
			CHECK(batch.is_empty() == false);
			CHECK(batch[0] == &b);
			CHECK(join.is<TagA>());
			CHECK(join.as<TagA>() == &a);
			CHECK(ptr_another_a == &another_tag_a);
			CHECK(ptr_another_b == &another_tag_b);
			CHECK(another_entity == EntityID(100));
			CHECK(ptr_transf == &transf);
		}
	}
}

TEST_CASE("[Modules][ECS] Test static query") {
	ECS::register_component<TagQueryTestComponent>();

	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagQueryTestComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent(Transform(Basis(), Vector3(0.0, 0.0, 23.0))));

	EntityID entity_3 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagQueryTestComponent());

	// Test `Without` filter.
	{
		Query<const TransformComponent, Without<TagQueryTestComponent>> query(&world);

		// This query fetches the entity that have only the `TransformComponent`.
		CHECK(query.has(entity_1) == false);
		CHECK(query.has(entity_2));
		CHECK(query.has(entity_3) == false);
		auto [transform, tag] = query[entity_2];
		CHECK(ABS(transform->transform.origin.z - 23.0) <= CMP_EPSILON);
		CHECK(tag == nullptr);
	}

	// Test `Maybe` filter.
	{
		Query<const TransformComponent, Maybe<TagQueryTestComponent>> query(&world);

		// This query fetches all entities but return nullptr when
		// `TagQueryTestComponent` is not set.
		{
			// Entity 1
			CHECK(query.has(entity_1));
			auto [transform, tag] = query[entity_1];
			CHECK(transform != nullptr);
			CHECK(tag != nullptr);
		}

		{
			// Entity 2
			CHECK(query.has(entity_2));
			auto [transform, tag] = query[entity_2];
			CHECK(transform != nullptr);
			CHECK(tag == nullptr);
		}

		{
			// Entity 3
			CHECK(query.has(entity_3));
			auto [transform, tag] = query[entity_3];
			CHECK(transform != nullptr);
			CHECK(tag != nullptr);
		}
	}
}

TEST_CASE("[Modules][ECS] Test static query deep nesting") {
	ECS::register_component<TestFixedSizeEvent>();

	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TagA())
								.with(TagB())
								.with(TagC())
								.with(TestFixedSizeEvent(20))
								.with(TransformComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TagA())
								.with(TagB())
								.with(TagC())
								.with(TestFixedSizeEvent(25))
								.with(TestFixedSizeEvent(30))
								.with(TransformComponent());

	world.get_storage<TagC>()->set_tracing_change(true);
	world.get_storage<TagC>()->notify_changed(entity_1);

	world.get_storage<TestFixedSizeEvent>()->set_tracing_change(true);
	world.get_storage<TestFixedSizeEvent>()->notify_changed(entity_2);

	// Test `EntityID`, `With` `Maybe`, `Without + Changed`.
	{
		// Take this only if `TagC` DOESN'T changed.
		Query<EntityID, TagA, Maybe<TagB>, Without<Changed<TagC>>> query(&world);

		// The `TagC` on the `Entity1` is changed, so the query doesn't fetches it.
		CHECK(query.has(entity_1) == false);

		// The `TagC` on the `Entity2` is not changed, so the query fetches it.
		CHECK(query.has(entity_2));

		auto [entity, tag_a, tag_b, tag_c] = query.new_get(entity_2);
		CHECK(entity == entity_2);
		CHECK(tag_a != nullptr);
		CHECK(tag_b != nullptr);
		CHECK(tag_c == nullptr);
	}

	// Test `EntityID`, `With`, `Maybe + Changed`.
	{
		// Take this only if `TagC` DOESN'T changed.
		Query<EntityID, TagA, Maybe<Changed<TagC>>> query(&world);

		CHECK(query.has(entity_1));
		CHECK(query.has(entity_2));

		// In Entity1 the tag is changed, we expect it.
		{
			auto [entity, tag_a, tag_c] = query.new_get(entity_1);
			CHECK(entity == entity_1);
			CHECK(tag_a != nullptr);
			CHECK(tag_c != nullptr);
		}

		// In Entity2 the tag is NOT changed, we expect nullptr.
		{
			auto [entity, tag_a, tag_c] = query.new_get(entity_2);
			CHECK(entity == entity_2);
			CHECK(tag_a != nullptr);
			CHECK(tag_c == nullptr);
		}
	}

	// Test nested Batch
	{
		Query<EntityID, TransformComponent, Batch<Maybe<Changed<TestFixedSizeEvent>>>> query(&world);

		CHECK(query.has(entity_1));
		CHECK(query.has(entity_2));

		// In Entity1 the event is NOT changed, we expect nullptr.
		{
			auto [entity, transf, event] = query.new_get(entity_1);
			CHECK(entity == entity_1);
			CHECK(transf != nullptr);
			CHECK(event.is_empty());
		}

		// In Entity2 the tag is NOT changed, we expect nullptr.
		{
			auto [entity, transf, event] = query.new_get(entity_2);
			CHECK(entity == entity_2);
			CHECK(transf != nullptr);
			CHECK(event.is_empty() == false);
			CHECK(event.get_size() == 2);
			CHECK(event[0]->number == 25);
			CHECK(event[1]->number == 30);
		}
	}

	// TODO test Join
	// TODO test Any
}
} // namespace godex_tests

/// Used to trace the query mutability access the storage.
template <class T>
class AccessTracerStorage : public Storage<T> {
	DenseVector<T> storage;

public:
	uint32_t count_insert = 0;
	uint32_t count_has = 0;
	uint32_t count_remove = 0;
	uint32_t count_get_mut = 0;
	uint32_t count_get_immut = 0;
	uint32_t count_clear = 0;

	virtual String get_type_name() const override {
		return "AccessTranceStorage";
	}

	virtual void insert(EntityID p_entity, const T &p_data) override {
		count_insert += 1;
		storage.insert(p_entity, p_data);
		StorageBase::notify_changed(p_entity);
	}

	virtual bool has(EntityID p_entity) const override {
		const_cast<AccessTracerStorage<T> *>(this)->count_has += 1;
		return storage.has(p_entity);
	}

	virtual T *get(EntityID p_entity, Space p_mode = Space::LOCAL) override {
		StorageBase::notify_changed(p_entity);
		count_get_mut += 1;
		return &storage.get(p_entity);
	}

	virtual const T *get(EntityID p_entity, Space p_mode = Space::LOCAL) const override {
		const_cast<AccessTracerStorage<T> *>(this)->count_get_immut += 1;
		return &storage.get(p_entity);
	}

	virtual void remove(EntityID p_entity) override {
		StorageBase::notify_updated(p_entity);
		count_remove += 1;
		storage.remove(p_entity);
	}

	virtual void clear() override {
		count_clear += 1;
		storage.clear();
		StorageBase::flush_changed();
	}

	virtual EntitiesBuffer get_stored_entities() const override {
		return EntitiesBuffer(storage.get_entities().size(), storage.get_entities().ptr());
	}
};

struct TestAccessMutabilityComponent1 {
	COMPONENT(TestAccessMutabilityComponent1, AccessTracerStorage)
	static void _bind_methods() {}
};

struct TestAccessMutabilityComponent2 {
	COMPONENT(TestAccessMutabilityComponent2, AccessTracerStorage)
	static void _bind_methods() {}
};

namespace godex_tests {
TEST_CASE("[Modules][ECS] Test query mutability.") {
	ECS::register_component<TestAccessMutabilityComponent1>();
	ECS::register_component<TestAccessMutabilityComponent2>();

	World world;

	world
			.create_entity()
			.with(TransformComponent())
			.with(TestAccessMutabilityComponent1());

	world
			.create_entity()
			.with(TransformComponent())
			.with(TestAccessMutabilityComponent2());

	// Make sure the query access to the storage with the correct mutability.

	AccessTracerStorage<TestAccessMutabilityComponent1> *storage1 = static_cast<AccessTracerStorage<TestAccessMutabilityComponent1> *>(world.get_storage<TestAccessMutabilityComponent1>());
	AccessTracerStorage<TestAccessMutabilityComponent2> *storage2 = static_cast<AccessTracerStorage<TestAccessMutabilityComponent2> *>(world.get_storage<TestAccessMutabilityComponent2>());

	storage1->set_tracing_change(true);
	storage2->set_tracing_change(true);

	// Just two insert.
	CHECK(storage1->count_insert == 1);
	CHECK(storage2->count_insert == 1);

	// Test mutable
	CHECK(storage1->count_get_mut == 0);

	Query<TestAccessMutabilityComponent1> query_test_mut(&world);
	query_test_mut.begin().operator*(); // Fetch the data.

	CHECK(storage1->count_get_mut == 1);
	CHECK(storage2->count_get_mut == 0);

	Query<Without<TestAccessMutabilityComponent1>, TestAccessMutabilityComponent2> query_test_without_mut(&world);
	query_test_without_mut.begin().operator*(); //Fetch the data.

	CHECK(storage1->count_get_mut == 1);
	CHECK(storage2->count_get_mut == 1);

	Query<Maybe<TestAccessMutabilityComponent1>, TransformComponent> query_test_maybe_mut(&world);
	query_test_maybe_mut.begin().operator*(); //Fetch the data.

	CHECK(storage1->count_get_mut == 2);

	Query<Changed<TestAccessMutabilityComponent1>, TransformComponent> query_test_changed_mut(&world);
	query_test_changed_mut.begin().operator*(); //Fetch the data.

	CHECK(storage1->count_get_mut == 3);

	// Test immutables

	CHECK(storage1->count_get_immut == 0);

	Query<const TestAccessMutabilityComponent1> query_test_immut(&world);
	query_test_immut.begin().operator*(); //Fetch the data.

	CHECK(storage1->count_get_mut == 3);
	CHECK(storage1->count_get_immut == 1);
	CHECK(storage2->count_get_immut == 0);

	Query<Without<const TestAccessMutabilityComponent1>, const TestAccessMutabilityComponent2> query_test_without_immut(&world);
	query_test_without_immut.begin().operator*(); //Fetch the data.

	CHECK(storage1->count_get_mut == 3);
	CHECK(storage1->count_get_immut == 1);
	CHECK(storage2->count_get_immut == 1);

	Query<Maybe<const TestAccessMutabilityComponent1>, const TransformComponent> query_test_maybe_immut(&world);
	query_test_maybe_immut.begin().operator*(); //Fetch the data.

	CHECK(storage1->count_get_mut == 3);
	CHECK(storage1->count_get_immut == 2);

	Query<Changed<const TestAccessMutabilityComponent1>, TransformComponent> query_test_changed_immut(&world);
	query_test_changed_immut.begin().operator*(); //Fetch the data.

	CHECK(storage1->count_get_mut == 3);
	CHECK(storage1->count_get_immut == 3);
}

TEST_CASE("[Modules][ECS] Test DynamicQuery fetch mutability.") {
	World world;

	world
			.create_entity()
			.with(TestAccessMutabilityComponent1());

	world
			.create_entity()
			.with(TestAccessMutabilityComponent2());

	// Make sure the query access to the storage with the correct mutability.

	AccessTracerStorage<TestAccessMutabilityComponent1> *storage1 = static_cast<AccessTracerStorage<TestAccessMutabilityComponent1> *>(world.get_storage<TestAccessMutabilityComponent1>());
	AccessTracerStorage<TestAccessMutabilityComponent2> *storage2 = static_cast<AccessTracerStorage<TestAccessMutabilityComponent2> *>(world.get_storage<TestAccessMutabilityComponent2>());

	// Just two insert.
	CHECK(storage1->count_insert == 1);
	CHECK(storage2->count_insert == 1);

	// Test mutable
	CHECK(storage1->count_get_mut == 0);

	godex::DynamicQuery query_test_mut;
	query_test_mut.with_component(TestAccessMutabilityComponent1::get_component_id(), true);
	query_test_mut.begin(&world);

	CHECK(storage1->count_get_mut == 1);
	CHECK(storage2->count_get_mut == 0);

	godex::DynamicQuery query_test_without_mut;
	query_test_without_mut.without_component(TestAccessMutabilityComponent1::get_component_id());
	query_test_without_mut.with_component(TestAccessMutabilityComponent2::get_component_id(), true);
	query_test_without_mut.begin(&world);

	CHECK(storage1->count_get_mut == 1);
	CHECK(storage2->count_get_mut == 1);

	// Make sure `maybe` alone doesn't fetch anything.
	godex::DynamicQuery query_test_maybe_alone_mut;
	query_test_maybe_alone_mut.maybe_component(TestAccessMutabilityComponent1::get_component_id(), true);
	query_test_maybe_alone_mut.begin(&world);

	CHECK(storage1->count_get_mut == 1);

	// Test immutables

	CHECK(storage1->count_get_immut == 0);

	godex::DynamicQuery query_test_immut;
	query_test_immut.with_component(TestAccessMutabilityComponent1::get_component_id(), false);
	query_test_immut.begin(&world);
	CHECK(storage1->count_get_mut == 1);
	CHECK(storage1->count_get_immut == 1);

	CHECK(storage2->count_get_immut == 0);
	godex::DynamicQuery query_test_without_immut;
	query_test_without_immut.without_component(TestAccessMutabilityComponent1::get_component_id());
	query_test_without_immut.with_component(TestAccessMutabilityComponent2::get_component_id(), false);
	query_test_without_immut.begin(&world);

	CHECK(storage1->count_get_mut == 1);
	CHECK(storage1->count_get_immut == 1);
	CHECK(storage2->count_get_immut == 1);

	godex::DynamicQuery query_test_maybe_alone_immut;
	query_test_maybe_alone_immut.maybe_component(TestAccessMutabilityComponent1::get_component_id(), false);
	query_test_maybe_alone_immut.begin(&world);

	CHECK(storage1->count_get_mut == 1);
	CHECK(storage1->count_get_immut == 1);
}

TEST_CASE("[Modules][ECS] Test DynamicQuery try fetch combination.") {
	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TestAccessMutabilityComponent1());

	world
			.create_entity()
			.with(TestAccessMutabilityComponent2());

	EntityID entity_3 = world
								.create_entity()
								.with(TestAccessMutabilityComponent1())
								.with(TestAccessMutabilityComponent2());

	{
		// Check `With`  and `With`.
		godex::DynamicQuery query;
		query.with_component(TestAccessMutabilityComponent1::get_component_id(), false);
		query.with_component(TestAccessMutabilityComponent2::get_component_id(), false);
		query.begin(&world);
		CHECK(query.has(entity_3));
	}

	{
		// Check `With`  and `Without`.
		godex::DynamicQuery query;
		query.with_component(TestAccessMutabilityComponent1::get_component_id(), false);
		query.without_component(TestAccessMutabilityComponent2::get_component_id());
		query.begin(&world);
		CHECK(query.has(entity_1));
	}

	{
		// Check `With`  and `Maybe`.
		godex::DynamicQuery query;
		query.with_component(TestAccessMutabilityComponent1::get_component_id(), false);
		query.maybe_component(TestAccessMutabilityComponent2::get_component_id());
		query.begin(&world);
		CHECK(query.has(entity_1));
		CHECK(query.has(entity_3));
	}

	{
		// Check `Maybe`.
		godex::DynamicQuery query;
		query.maybe_component(TestAccessMutabilityComponent2::get_component_id());
		query.begin(&world);
		// Nothing to fetch, because `Maybe` alone is meaningless.
		CHECK(query.is_not_done() == false);
	}

	{
		// Check `Without`.
		godex::DynamicQuery query;
		query.without_component(TestAccessMutabilityComponent2::get_component_id());
		query.begin(&world);
		// Nothing to fetch, because `Without` alone is meaningless.
		CHECK(query.is_not_done() == false);
	}

	{
		// Check `Maybe` and `Without`.
		godex::DynamicQuery query;
		query.maybe_component(TestAccessMutabilityComponent1::get_component_id());
		query.without_component(TestAccessMutabilityComponent2::get_component_id());
		query.begin(&world);
		// Nothing to fetch, because `Maybe` and `Without` are meaningless alone.
		CHECK(query.is_not_done() == false);
	}
}

TEST_CASE("[Modules][ECS] Test DynamicQuery changed.") {
	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TestAccessMutabilityComponent1());

	world.get_storage(TestAccessMutabilityComponent1::get_component_id())->set_tracing_change(true);

	{
		// Trigger changed.
		godex::DynamicQuery query;
		query.with_component(TestAccessMutabilityComponent1::get_component_id(), true);
		query.begin(&world);
		CHECK(query.has(entity_1));
	}

	{
		// Check I can access changed.
		godex::DynamicQuery query;
		query.changed_component(TestAccessMutabilityComponent1::get_component_id(), false);
		query.begin(&world);
		CHECK(query.has(entity_1));
	}

	world.get_storage(TestAccessMutabilityComponent1::get_component_id())->flush_changed();

	{
		// Take the component again, without triggering changed.
		godex::DynamicQuery query;
		query.with_component(TestAccessMutabilityComponent1::get_component_id(), false);
		query.begin(&world);
		CHECK(query.has(entity_1));
	}

	{
		// Check changed is gone.
		godex::DynamicQuery query;
		query.changed_component(TestAccessMutabilityComponent1::get_component_id(), false);
		query.begin(&world);
		CHECK(query.is_not_done() == false);
	}
}

TEST_CASE("[Modules][ECS] Test static query check query type fetch.") {
	World world;

	{
		Query<const TransformComponent, Maybe<TagQueryTestComponent>> query(&world);

		SystemExeInfo info;
		query.get_components(info);

		CHECK(info.mutable_components.find(TagQueryTestComponent::get_component_id()) != nullptr);
		CHECK(info.immutable_components.find(TransformComponent::get_component_id()) != nullptr);
	}

	{
		Query<TransformComponent, Maybe<const TagQueryTestComponent>> query(&world);

		SystemExeInfo info;
		query.get_components(info);

		CHECK(info.mutable_components.find(TransformComponent::get_component_id()) != nullptr);

		CHECK(info.immutable_components.find(TagQueryTestComponent::get_component_id()) != nullptr);
	}

	{
		Query<Without<TransformComponent>, Maybe<const TagQueryTestComponent>> query(&world);

		SystemExeInfo info;
		query.get_components(info);

		CHECK(info.mutable_components.size() == 0);

		// `Without` filter collects the data always immutable.
		CHECK(info.immutable_components.find(TransformComponent::get_component_id()) != nullptr);
		CHECK(info.immutable_components.find(TagQueryTestComponent::get_component_id()) != nullptr);
	}

	{
		Query<Changed<TransformComponent>, Changed<const TagQueryTestComponent>> query(&world);

		SystemExeInfo info;
		query.get_components(info);

		CHECK(info.mutable_components.find(TransformComponent::get_component_id()) != nullptr);
		CHECK(info.immutable_components.find(TagQueryTestComponent::get_component_id()) != nullptr);
		CHECK(info.need_changed.find(TransformComponent::get_component_id()) != nullptr);
		CHECK(info.need_changed.find(TagQueryTestComponent::get_component_id()) != nullptr);
	}
}

TEST_CASE("[Modules][ECS] Test static query filter no storage.") {
	World world;

	{
		Query<Without<TagQueryTestComponent>, TransformComponent> query(&world);

		// No storage, make sure this returns immediately.
		CHECK(query.count() == 0);
	}

	{
		Query<Maybe<TagQueryTestComponent>, TransformComponent> query(&world);

		// No storage, make sure this returns immediately.
		CHECK(query.count() == 0);
	}
}

TEST_CASE("[Modules][ECS] Test dynamic query") {
	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagQueryTestComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent());

	EntityID entity_3 = world
								.create_entity()
								.with(TransformComponent())
								.with(TagQueryTestComponent());

	{
		godex::DynamicQuery query;
		query.with_component(ECS::get_component_id("TransformComponent"), true);
		query.with_component(ECS::get_component_id("TagQueryTestComponent"), false);

		CHECK(query.is_valid());

		// Test process.
		query.begin(&world);

		// Entity 1
		CHECK(query.has(entity_1));
		query.fetch(entity_1);

		{
			CHECK(query.access_count() == 2);
			CHECK(query.get_access(0)->is_mutable());
			CHECK(query.get_access(1)->is_mutable() == false);
			query.get_access(0)->set("transform", Transform(Basis(), Vector3(100.0, 100.0, 100.0)));
		}

		CHECK(query.has(entity_2) == false);

		// Entity 3 (Entity 2 is skipped because it doesn't fulfil the query)
		CHECK(query.has(entity_3));
		query.fetch(entity_3);

		{
			CHECK(query.access_count() == 2);
			CHECK(query.get_access(0)->is_mutable());
			CHECK(query.get_access(1)->is_mutable() == false);
			query.get_access(0)->set("transform", Transform(Basis(), Vector3(200.0, 200.0, 200.0)));
		}

		// Nothing more to do at this point.
		query.end();
	}

	{
		godex::DynamicQuery query;
		query.with_component(ECS::get_component_id("TransformComponent"), false);

		// Test process.
		query.begin(&world);

		// Entity 1
		CHECK(query.is_not_done());
		CHECK(query.get_current_entity_id() == entity_1);

		{
			CHECK(query.access_count() == 1);
			CHECK(query.get_access(0)->is_mutable() == false);
			const Transform t = query.get_access(0)->get("transform");
			// Check if the entity_1 is changed.
			CHECK(t.origin.x - 100.0 <= CMP_EPSILON);
		}

		query.next();

		// Entity 2
		CHECK(query.is_not_done());
		CHECK(query.get_current_entity_id() == entity_2);
		{
			CHECK(query.access_count() == 1);
			CHECK(query.get_access(0)->is_mutable() == false);
			const Transform t = query.get_access(0)->get("transform");
			// Make sure the entity_2 is not changed.
			CHECK(t.origin.x <= CMP_EPSILON);
		}

		query.next();

		// Entity 3
		CHECK(query.is_not_done());
		CHECK(query.get_current_entity_id() == entity_3);
		{
			CHECK(query.access_count() == 1);
			CHECK(query.get_access(0)->is_mutable() == false);
			const Transform t = query.get_access(0)->get("transform");
			// Make sure the entity_3 is changed.
			CHECK(t.origin.x - 200.0 <= CMP_EPSILON);
		}

		query.next();

		CHECK(query.is_not_done() == false);
		CHECK(query.count() == 3);
		query.end();
	}

	{
		// Check the API `without_component()`.
		godex::DynamicQuery query;
		query.with_component(TransformComponent::get_component_id());
		query.without_component(TagQueryTestComponent::get_component_id());

		query.begin(&world);

		// This query fetches the entity that have only the `TransformComponent`.
		CHECK(query.is_not_done());
		CHECK(query.get_current_entity_id() == entity_2);

		query.next();

		// Now it's done
		CHECK(query.is_not_done() == false);
		CHECK(query.count() == 1);
		query.end();
	}

	{
		// Check the API `maybe_component()`.
		godex::DynamicQuery query;
		query.with_component(TransformComponent::get_component_id());
		query.maybe_component(TagQueryTestComponent::get_component_id());
		query.build();

		query.begin(&world);

		// This query fetches the entity that have only the `TransformComponent`.
		CHECK(query.is_not_done());
		CHECK(query.get_current_entity_id() == entity_1);
		CHECK(query.get_access(0)->get_target() != nullptr);
		CHECK(query.get_access(1)->get_target() != nullptr);
		query.next();

		CHECK(query.is_not_done());
		CHECK(query.get_current_entity_id() == entity_2);
		CHECK(query.get_access(0)->get_target() != nullptr);
		CHECK(query.get_access(1)->get_target() == nullptr);
		query.next();

		CHECK(query.is_not_done());
		CHECK(query.get_current_entity_id() == entity_3);
		CHECK(query.get_access(0)->get_target() != nullptr);
		CHECK(query.get_access(1)->get_target() != nullptr);
		query.next();

		// Now it's done
		CHECK(query.is_not_done() == false);
		CHECK(query.count() == 3);
		query.end();
	}
}

TEST_CASE("[Modules][ECS] Test dynamic query with dynamic storages.") {
	LocalVector<ScriptProperty> props;
	props.push_back({ PropertyInfo(Variant::INT, "variable_1"), 1 });
	props.push_back({ PropertyInfo(Variant::BOOL, "variable_2"), false });
	props.push_back({ PropertyInfo(Variant::VECTOR3, "variable_3"), Vector3() });

	const uint32_t test_dyn_component_id = ECS::register_script_component(
			"TestDynamicQueryComponent1.gd",
			props,
			StorageType::DENSE_VECTOR);

	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(test_dyn_component_id, Dictionary());

	EntityID entity_2 = world
								.create_entity()
								.with(test_dyn_component_id, Dictionary());

	godex::DynamicQuery query;
	query.with_component(test_dyn_component_id, true);

	CHECK(query.is_valid());

	// Test process.
	query.begin(&world);

	// Entity 1
	CHECK(query.is_not_done());
	CHECK(query.get_current_entity_id() == entity_1);

	{
		CHECK(query.access_count() == 1);
		CHECK(query.get_access(0)->is_mutable());
		query.get_access(0)->set("variable_1", 20);
		query.get_access(0)->set("variable_2", true);
		query.get_access(0)->set("variable_3", Vector2(10., 10.)); // Test if wrong type is still handled: `variable_3` is a `Vector3`.
	}

	query.next();

	// Entity 2
	CHECK(query.is_not_done());
	CHECK(query.get_current_entity_id() == entity_2);

	{
		CHECK(query.access_count() == 1);
		CHECK(query.get_access(0)->is_mutable());
		query.get_access(0)->set("variable_1", 30);
		query.get_access(0)->set("variable_2", true);
		query.get_access(0)->set("variable_3", Vector3(10.0, 0, 0));
	}

	query.next();
	CHECK(query.is_not_done() == false);
	query.end();

	// ~~ Make sure the data got written using another immutable query. ~~

	query.reset();
	query.with_component(test_dyn_component_id, false);

	query.begin(&world);

	// Entity 1
	CHECK(query.is_not_done());
	CHECK(query.get_current_entity_id() == entity_1);

	{
		CHECK(query.access_count() == 1);
		CHECK(query.get_access(0)->is_mutable() == false);
		CHECK(query.get_access(0)->get("variable_1") == Variant(20));
		CHECK(query.get_access(0)->get("variable_2") == Variant(true));
		// Make sure this doesn't changed, since we submitted a wrong value.
		CHECK(query.get_access(0)->get("variable_3") == Variant(Vector3()));
	}

	query.next();

	// Entity 2
	CHECK(query.is_not_done());
	CHECK(query.get_current_entity_id() == entity_2);

	{
		CHECK(query.access_count() == 1);
		CHECK(query.get_access(0)->is_mutable() == false);
		CHECK(query.get_access(0)->get("variable_1") == Variant(30));
		CHECK(query.get_access(0)->get("variable_2") == Variant(true));
		CHECK(ABS(query.get_access(0)->get("variable_3").operator Vector3().x - 10.0) < CMP_EPSILON);

		// Try to mutate an immutable value.
		query.get_access(0)->set("variable_2", false);
		CHECK(query.get_access(0)->get("variable_2") != Variant(false));
	}

	query.next();
	CHECK(query.is_not_done() == false);
	query.end();
}

TEST_CASE("[Modules][ECS] Test invalid dynamic query.") {
	godex::DynamicQuery query;

	// Add an invalid component.
	query.with_component(ECS::get_component_id("ThisComponentDoesntExists"));

	CHECK(query.is_valid() == false);

	// Reset the query.
	query.reset();

	// Build it again but this time valid.
	query.with_component(ECS::get_component_id("TransformComponent"));
	CHECK(query.is_valid());
}

TEST_CASE("[Modules][ECS] Test query with event.") {
	ECS::register_component<TestEvent>();

	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TransformComponent())
								.with(TestEvent(50))
								.with(TestEvent(38));

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent())
								.with(TestFixedSizeEvent(645))
								.with(TestFixedSizeEvent(33))
								.with(TestFixedSizeEvent(78))
								.with(TestFixedSizeEvent(52));

	EntityID entity_3 = world
								.create_entity()
								.with(TransformComponent())
								.with(TestEvent());

	world.get_storage<TestEvent>()->set_tracing_change(true);

	// Try the first query with dynamic sized batch storage.
	{
		Query<EntityID, TransformComponent, Batch<const TestEvent>> query(&world);

		{
			CHECK(query.has(entity_1));
			auto [entity, transform, event] = query[entity_1];

			CHECK(entity_1 == entity);

			CHECK(transform != nullptr);

			CHECK(event.get_size() == 2);
			CHECK(event[0]->number == 50);
			CHECK(event[1]->number == 38);
		}

		CHECK(query.has(entity_2) == false);

		{
			CHECK(query.has(entity_3));
			auto [entity, transform, event] = query[entity_3];

			CHECK(entity_3 == entity);

			CHECK(transform != nullptr);

			CHECK(event.get_size() == 1);
			CHECK(event[0]->number == 0);
		}
	}

	// Try the second query with fixed sized batch storage.
	{
		Query<EntityID, TransformComponent, Batch<const TestFixedSizeEvent>> query(&world);

		{
			CHECK(query.has(entity_2));
			auto [entity, transform, event] = query[entity_2];

			CHECK(entity_2 == entity);

			CHECK(transform != nullptr);

			CHECK(event.get_size() == 2);
			CHECK(event[0]->number == 645);
			CHECK(event[1]->number == 33);
		}
	}

	// Try the second query with fixed sized batch storage.
	{
		Query<EntityID, TransformComponent, Batch<const TestFixedSizeEvent>> query(&world);

		{
			CHECK(query.has(entity_2));
			auto [entity, transform, event] = query[entity_2];

			CHECK(entity_2 == entity);

			CHECK(transform != nullptr);

			CHECK(event.get_size() == 2);
			CHECK(event[0]->number == 645);
			CHECK(event[1]->number == 33);
		}
	}

	// Test sub filters
	{
		// We always fetched with `const`, make sure no changes got triggered.
		{
			Query<EntityID, Batch<Changed<const TestEvent>>> query(&world);
			CHECK(query.count() == 0);
		}

		// Trigger the changed event now, by fetching MUTABLE.
		{
			Query<EntityID, Batch<TestEvent>> query(&world);
			uint32_t c = 0;
			// Using the for loop to trigger the changed event.
			for (auto [e, te] : query) {
				CHECK(e != EntityID());
				CHECK(te.is_empty() == false);
				c += 1;
			}
			CHECK(c == 2);
		}

		// Try again with the changed now.
		{
			Query<EntityID, Batch<Changed<const TestEvent>>> query(&world);
			CHECK(query.count() == 2); // Two entities.
		}
	}
}

TEST_CASE("[Modules][ECS] Test query random Entity access.") {
	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TransformComponent())
								.with(TestEvent(50));

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent());

	EntityID entity_3 = world
								.create_entity()
								.with(TransformComponent())
								.with(TestEvent());

	Query<EntityID, TransformComponent, TestEvent> query(&world);

	{
		CHECK(query.has(entity_1));
		auto [entity, transform, event] = query[entity_1];
		CHECK(entity_1 == entity);
		CHECK(event->number == 50);
	}

	CHECK(query.has(entity_2) == false);

	{
		CHECK(query.has(entity_3));
		auto [entity, transform, event] = query[entity_3];
		CHECK(entity_3 == entity);
		CHECK(event->number == 0);
	}
}

TEST_CASE("[Modules][ECS] Test static query count.") {
	World world;

	world
			.create_entity()
			.with(TagQueryTestComponent())
			.with(TransformComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent());

	world
			.create_entity()
			.with(TestFixedSizeEvent())
			.with(TransformComponent());

	{
		Query<const TransformComponent, TagQueryTestComponent> query(&world);
		CHECK(query.count() == 1);
	}
	{
		Query<const TransformComponent, Without<TagQueryTestComponent>, Without<TestFixedSizeEvent>> query(&world);
		CHECK(query.count() == 1);
		CHECK(query.has(entity_2));
	}
	{
		Query<TestFixedSizeEvent> query(&world);
		CHECK(query.count() == 1);
	}
	{
		Query<const TransformComponent> query(&world);
		CHECK(query.count() == 3);
	}
}

TEST_CASE("[Modules][ECS] Test static query Any filter.") {
	World world;

	EntityID entity_1 = world
								.create_entity()
								.with(TagA())
								.with(TransformComponent());

	EntityID entity_2 = world
								.create_entity()
								.with(TransformComponent());

	EntityID entity_3 = world
								.create_entity()
								.with(TagB())
								.with(TransformComponent());

	EntityID entity_4 = world
								.create_entity()
								.with(TagC())
								.with(TransformComponent());

	{
		Query<TransformComponent, Any<const TagA, const TagB, TagC>> query(&world);

		CHECK(query.has(entity_1));
		CHECK(query.has(entity_2) == false);
		CHECK(query.has(entity_3));
		CHECK(query.has(entity_4));

		// Fetch entity_1
		{
			auto [transform, tag] = query[entity_1];
			CHECK(tag.is<const TagA>());
			CHECK(tag.is<const TagB>() == false);
			CHECK(tag.is<const TagC>() == false);
			CHECK(tag.is<TagA>() == false);
			CHECK(tag.is<TagB>() == false);
			CHECK(tag.is<TagC>() == false);

			const TagA *tag_a = tag.as<const TagA>();
			CHECK(tag_a != nullptr);
		}

		// Fetch entity_2
		{
			CHECK(query.has(entity_2) == false);
		}

		// Fetch entity_3
		{
			auto [transform, tag] = query[entity_3];
			CHECK(tag.is<const TagA>() == false);
			CHECK(tag.is<const TagB>());
			CHECK(tag.is<const TagC>() == false);
			CHECK(tag.is<TagA>() == false);
			CHECK(tag.is<TagB>() == false);
			CHECK(tag.is<TagC>() == false);

			const TagB *tag_b = tag.as<const TagB>();
			CHECK(tag_b != nullptr);
		}

		// Fetch entity_4
		{
			auto [transform, tag] = query[entity_4];
			CHECK(tag.is<const TagA>() == false);
			CHECK(tag.is<const TagB>() == false);
			CHECK(tag.is<const TagC>() == false);
			CHECK(tag.is<TagA>() == false);
			CHECK(tag.is<TagB>() == false);
			CHECK(tag.is<TagC>());

			TagC *tag_c = tag.as<TagC>();
			CHECK(tag_c != nullptr);
		}
	}

	world.get_storage<TransformComponent>()->set_tracing_change(true);
	world.get_storage<TransformComponent>()->notify_changed(entity_1);
	world.get_storage<TransformComponent>()->notify_changed(entity_2);
	world.get_storage<TransformComponent>()->notify_changed(entity_3);
	world.get_storage<TransformComponent>()->notify_changed(entity_4);

	world.get_storage<TagB>()->set_tracing_change(true);
	world.get_storage<TagB>()->notify_changed(entity_3);

	world.get_storage<TagC>()->set_tracing_change(true);
	world.get_storage<TagC>()->notify_changed(entity_4);

	// Test `Any` with `Changed` filter.
	{
		Query<Changed<TransformComponent>, Any<Changed<const TagA>, Changed<const TagB>, Changed<TagC>>> query(&world);

		CHECK(query.has(entity_1) == false);
		CHECK(query.has(entity_2) == false);
		CHECK(query.has(entity_3));
		CHECK(query.has(entity_4));

		{
			auto [transform, tag] = query[entity_3];
			CHECK(tag.is<const TagB>());
		}

		{
			auto [transform, tag] = query[entity_4];
			CHECK(tag.is<TagC>());
		}
	}

	// Test `Any` with and without `Changed` filter.
	{
		Query<Changed<TransformComponent>, Any<const TagA, Changed<const TagB>, Changed<TagC>>> query(&world);

		CHECK(query.has(entity_1));
		CHECK(query.has(entity_2) == false);
		CHECK(query.has(entity_3));
		CHECK(query.has(entity_4));

		{
			auto [transform, tag] = query[entity_1];
			CHECK(tag.is<const TagA>());
		}

		{
			auto [transform, tag] = query[entity_3];
			CHECK(tag.is<const TagB>());
		}

		{
			auto [transform, tag] = query[entity_4];
			CHECK(tag.is<TagC>());
		}
	}

	// Test `Any` with `Without` filter.
	// Note: This test is here just for validation, but doesn't make much sense.
	{
		Query<Changed<TransformComponent>, Any<Without<TagA>, Without<const TagB>, Without<TagC>>> query(&world);

		// Since `Any` needs just one filter to be satisfied, all are valid.
		CHECK(query.has(entity_1));
		CHECK(query.has(entity_2));
		CHECK(query.has(entity_3));
		CHECK(query.has(entity_4));

		{
			auto [transform, tag] = query[entity_1];
			CHECK(transform != nullptr);
			CHECK(tag.is_null());
		}
		{
			auto [transform, tag] = query[entity_2];
			CHECK(transform != nullptr);
			CHECK(tag.is_null());
		}
		{
			auto [transform, tag] = query[entity_3];
			CHECK(transform != nullptr);
			CHECK(tag.is_null());
		}
		{
			auto [transform, tag] = query[entity_4];
			CHECK(transform != nullptr);
			CHECK(tag.is_null());
		}
	}
}

TEST_CASE("[Modules][ECS] Test static query Group filter.") {
	//World world;

	///*EntityID entity_1 =*/world
	//		.create_entity()
	//		.with(TagA())
	//		.with(TransformComponent());

	///*EntityID entity_2 =*/world
	//		.create_entity()
	//		.with(TransformComponent());

	//EntityID entity_3 = world
	//							.create_entity()
	//							.with(TagB())
	//							.with(TransformComponent());

	//EntityID entity_4 = world
	//							.create_entity()
	//							.with(TagC())
	//							.with(TransformComponent());

	//world.get_storage<TagA>()->set_tracing_change(true);
	//world.get_storage<TagB>()->set_tracing_change(true);
	//world.get_storage<TagC>()->set_tracing_change(true);
	//world.get_storage<TransformComponent>()->set_tracing_change(true);

	//world.get_storage<TagC>()->notify_changed(entity_4);
	//world.get_storage<TransformComponent>()->notify_changed(entity_3);

	//{
	//	//Query<EntityID, Group<TransformComponent, Any<TagA, TagB, TagC>>> query(&world);
	//	Query<EntityID, Group<Changed<TransformComponent>, Any<Changed<TagA>, Changed<TagB>, Changed<TagC>>>> query(&world);

	//	auto it = query.begin();
	//	{
	//		auto [entity, group] = it.operator*();
	//		auto [transf, tag] = group;
	//		CHECK(entity == entity_3);
	//	}

	//	{
	//		auto [entity, group] = it.operator*();
	//		auto [transf, tag] = group;
	//		CHECK(entity == entity_4);
	//	}
	//}
}
} // namespace godex_tests

#endif // TEST_ECS_QUERY_H
