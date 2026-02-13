// Copyright 2026 atyxeut
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// general template metaprogramming utilities
export module aty.gatk.util.tmp;

import std;

namespace aty::gatk::tmp {

template <typename TFrom, typename TTo, bool = std::is_const_v<TFrom>, bool = std::is_volatile_v<TFrom>>
struct claim_cv_selector;

// branch 1: has both cv qualifiers
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, true, true>
{
  using type = std::add_cv_t<TTo>;
};

// branch 2: has only const qualifier
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, true, false>
{
  using type = std::add_const_t<TTo>;
};

// branch 3: has only volatile qualifier
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, false, true>
{
  using type = std::add_volatile_t<TTo>;
};

// branch 4: has no cv qualifiers
template <typename TFrom, typename TTo>
struct claim_cv_selector<TFrom, TTo, false, false>
{
  using type = TTo;
};

// extract the cv-qualifiers of TFrom and apply them to TTo
export template <typename TFrom, typename TTo>
using claim_cv = claim_cv_selector<TFrom, TTo>;

export template <typename TFrom, typename TTo>
using claim_cv_t = claim_cv<TFrom, TTo>::type;

} // namespace aty::gatk::tmp
