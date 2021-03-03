#pragma once

#include <utility>

namespace photon {
	template <std::size_t index, typename type>
	class tupleval {
	public:
		tupleval(const type &v) {
			val = v;
		}
		tupleval(type &&v) {
			val = std::move(v);
		}
		tupleval() : val() {}

		type &get() {
			return val;
		}
		const type &get() const {
			return val;
		}

		type val;
	};

	template <std::size_t index, typename... elements>
	class tuplebase {
	};

	template <std::size_t index, typename type, typename... elements>
	class tuplebase<index, type, elements...> : public tupleval<index, type>, public tuplebase<index + 1, elements...> {
	public:
		tuplebase() : tupleval<index, type>(), tuplebase<index + 1, elements...>() {}
		tuplebase(const tuplebase<index, elements...> &other) : tupleval<index, type>(other.tubleval), tuplebase<index + 1, elements...>(other.tuplebase) {}
		tuplebase(type &&arg, elements &&... args) : tupleval<index, type>(std::forward<type>(arg)), tuplebase<index + 1, elements...>(std::forward<elements>(args)...) {}
		tuplebase(const type &arg, const elements &... args) : tupleval<index, type>(arg), tuplebase<index + 1, elements...>(args...) {}
	};

	template <std::size_t index, typename L, typename... Args>
	struct extract_type_at {
		using type = typename extract_type_at<index - 1, Args...>::type;
	};

	template <typename L, typename... Args>
	struct extract_type_at<0, L, Args...> {
		using type = L;
	};

	template <typename... elements>
	class tuple : public tuplebase<0, elements...> {
	public:
		tuple() : tuplebase<0, elements...>() {}
		tuple(const tuple<elements...> &other) : tuplebase<0, elements...>(dynamic_cast<const tuplebase<0, elements...>&>(other)) {}
		tuple(elements &&... args) : tuplebase<0, elements...>(std::forward<elements>(args)...) {}
		tuple(const elements &... args) : tuplebase<0, elements...>(args...) {}

		template <std::size_t index>
		auto& get() {
			return (static_cast<tupleval<index, typename extract_type_at<index, elements...>::type> &>(*this)).get();
		}

		template <std::size_t index>
		const auto& get() const {
			return (static_cast<const tupleval<index, typename extract_type_at<index, elements...>::type> &>(*this)).get();
		}
	};
}

namespace std {
	template <typename ...Components>
	struct tuple_size<photon::tuple<Components...>> : integral_constant<size_t, sizeof...(Components)> {};

	template<size_t N, typename ...Components>
	struct tuple_element<N, photon::tuple<Components...>> {
		using type = decltype(declval<photon::tuple<Components...>>().template get<N>());
	};

	template<size_t N, typename ...Components>
	typename tuple_element<N, photon::tuple<Components...>>::type& get(photon::tuple<Components...> &t) {
		return t.template get<N>();
	}

	template<size_t N, typename ...Components>
	const typename tuple_element<N, photon::tuple<Components...>>::type& get(const photon::tuple<Components...> &t) {
		return t.template get<N>();
	}
}
