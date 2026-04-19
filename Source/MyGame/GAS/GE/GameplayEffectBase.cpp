// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffectBase.h"


// 1. Granted Tags (给角色挂标签) → 最常用！
// Granted Tags = 给角色挂状态
// 作用：
// GE 激活时 → 给角色加上这些 Tag
// GE 结束时 → 自动移除这些 Tag
// Spec.Def->InheritableOwnedTagsContainer.Added
// Spec.Def->InheritableOwnedTagsContainer.CombinedTags

//2. Gameplay Effect Asset Tags (GE 自身标签)

// Gameplay Effect Asset Tags = 给 GE 自己分类
// Spec.Def->InheritableGameplayEffectTags.Added

// 3. Application Tag Requirements (激活条件标签)
// 满足这些标签，GE 才能生效
// 4. Removal Tag Requirements (移除条件标签)
// 出现这些标签 → GE 自动被移除
// 5. Ongoing Tag Requirements (持续条件标签)
// 必须保持这些标签，否则 GE 立刻失效


// 当你尝试给角色应用一个 GE时，引擎会按顺序检查 3 个标签条件：
// 1. 先检查 Application Tag Requirements 与的关系
// 不满足 → 直接拒绝应用
// 2. 再检查 Removal Tag Requirements 或关系
// 已经满足 → 直接拒绝应用！
// （因为 “还没出生就该死”）
// 3. 最后检查 Ongoing Tag Requirements 与关系
// 不满足 → 加上去瞬间又被删掉


// Gameplay Effect Asset Tags (GE 自身标签)
// 1. 在代码里批量识别：这是 “冷却 / 增益 / 减益 / 控制”
// 比如你定义这些标签：
// EffectType.Cooldown
// EffectType.Buff
// EffectType.Debuff
// EffectType.Stun
// EffectType.Damage
// 然后你在所有冷却 GE 上都加上：
// Gameplay Effect Asset Tags = EffectType.Cooldown

// 2. 批量移除某一类效果
// 3. 防止重复叠加同类效果
// 4. UI 显示分类（Buff 图标）