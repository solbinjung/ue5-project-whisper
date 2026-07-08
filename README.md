# 🔫 Project: WHISPER

> **개발 기간:** 2026.03 ~ 2026.05 (3개월)<br>
> **개발 형태:** 1인 개발<br>
> **기술 스택:** Unreal Engine 5, C++<br>
> **주요 기술:** Gameplay Framework, Behavior Tree, Animation Blueprint, ApplyPointDamage, Two Bone IK<br>
> **시연 영상:** <https://youtu.be/wgEiBY0ZIWs><br>

어둠에 잠긴 SF 수처리 시설에서 생존하는 TPS 게임으로, Gameplay Framework 기반의 객체지향 설계와 Behavior Tree를 활용한 AI 시스템 구현에 중점을 두었습니다.

---

## 📂 프로젝트 구조

```text
Source/UE01
├── Player
│   ├── TPSCharacter           # 플레이어 상태, 전투, 무기 장착 및 IK
│   ├── TPSPlayerController    # 입력 처리(이동, 공격, 조준, 무기 교체)
│   ├── PlayerInterface        # 플레이어 HUD
│   └── HealthBarWidget        # 플레이어/적 체력 UI
│
├── Weapon
│   ├── TPSWeapon              # 무기 공통 로직(C++)
│   │                            # └─ 발사, 재장전, 탄약, 반동, 이펙트
│   ├── TPSWeaponInterface     # 공격, 재장전 인터페이스
│   └── AN_EndReload           # 재장전 완료 AnimNotify
│
└── Enemy
      ├── TPSEnemy               # 적 상태 및 전투
      ├── TPSEnemyAIController   # Behavior Tree 제어
      ├── BTTask_Attack          # 공격 Task
      └── TPSEnemySpawner        # 트리거 기반 적 생성

Content
├── Blueprints
│   └── Weapons
│        ├── BP_Rifle
│        ├── BP_Pistol
│        └── BP_Shotgun
│       
└── AI
     ├── Behavior Tree
     └── Blackboard

```

---

## 💡 주요 구현 기능

## 1. 무기 시스템 (C++)

### 객체지향 기반 무기 설계

<p align="center">
<img src="Images/weapons.png" width="600" alt="무기 설계">
</p>

* `ATPSWeapon`을 공통 무기 클래스로 구현하여 **발사, 재장전, 탄약 관리, 반동** 등 공통 로직을 관리했습니다.
* 라이플, 권총, 산탄총은 `ATPSWeapon`을 상속한 **Blueprint**에서 구현하고 **공격력, 연사 속도, 탄 퍼짐, 메쉬, Niagara 이펙트** 등을 각각 설정하여 코드 수정 없이 무기를 추가할 수 있도록 설계했습니다.

### 피격 판정 및 시각 효과

* `ApplyPointDamage`를 활용하여 공격 판정과 피해 처리를 분리하고 피격 대상이 자체적으로 데미지를 처리하도록 구현했습니다.
* `MuzzleSocket`을 기준으로 **Niagara Muzzle Flash**와 **Tracer**를 생성하여 발사 효과를 구현했습니다.

---

## 2. TPS 애니메이션 시스템

### 조준 및 이동 애니메이션

* **8방향 BlendSpace**, **Aim Offset**, **Turn In Place**를 적용하여 이동과 조준 상태에 따라 자연스럽게 전환되는 TPS 애니메이션을 구현했습니다.
* 조준 방향과 이동 방향을 분리하여 캐릭터가 이동 중에도 자연스럽게 조준할 수 있도록 구현했습니다. 

### 무기별 왼손 IK

* 모든 무기 메쉬에 LeftHandSocket을 추가하고, 소켓 Transform을 AnimBP로 전달했습니다.
* **Two Bone IK**를 적용하여 무기 종류와 크기가 달라도 항상 자연스럽게 총기 파지 모션을 유지하도록 구현했습니다.

---

## 3. Behavior Tree 기반 AI

### 상태 기반 AI

<p align="center">
<img src="Images/behaviorTree.png" width="500" alt="AI 비헤이비어 트리 및 상태 전이 설계">
</p>

* **Behavior Tree**와 **Blackboard**를 활용하여 탐색, 추격, 공격 상태를 관리했습니다.
* 적 사망 시 `AIController::StopLogic()`을 호출하여 Behavior Tree를 즉시 종료하도록 구현했습니다.

<details>
<summary>핵심 코드 보기 (TickTask)</summary>

```cpp
void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ATPSEnemy* Enemy = Cast<ATPSEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	
        if (Enemy && Enemy->GetMesh()->GetAnimInstance())
	{	
		UAnimMontage* AttackMontage = Enemy->GetAttackMontage();
		
                if (AttackMontage && !Enemy->GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

```

`TickTask()`에서 공격 몽타주의 재생 여부를 실시간으로 확인하고 애니메이션이 종료되면 `FinishLatentTask()`를 호출하여 Behavior Tree가 다음 상태로 즉시 전이되도록 구현했습니다.

</details>


### 콤보 공격

* Blackboard의 `IsInRange` 값을 기반으로 다음 콤보 공격 여부를 결정하고 `Montage_SetNextSection()`을 통해 다음 섹션으로 연결하여 자연스러운 연속 공격을 구현했습니다.

<details>
<summary>핵심 코드 보기 (CheckNextCombo)</summary>

```cpp
void ATPSEnemy::CheckNextCombo()
{
    bool bShouldContinueCombo =
        EnemyController->GetBlackboardComponent()
        ->GetValueAsBool(TEXT("IsInRange"));

    if (bShouldContinueCombo)
    {
        AttackIndex = (AttackIndex + 1) % MaxComboCount;

        AnimInstance->Montage_SetNextSection(
            CurrentSectionName,
            NextSectionName,
            OnAttack);
    }
    else
    {
        AttackIndex = 0;
    }
}

```
</details>

---

## 💡 기술적 문제 해결

### 1. Behavior Tree의 Wait 노드로 인한 AI 상태 전이 지연 해결

* **문제 상황**: 적 Behavior Tree에서 공격 후 `Wait` 노드를 사용하여 행동 시간을 제어했지만, 공격 몽타주의 실제 재생 시간과 대기 시간이 일치하지 않아 공격이 끊기거나 불필요한 대기 시간이 발생했습니다.
* **원인 분석**: `Wait` 노드는 고정된 시간만 대기하기 때문에 공격 몽타주의 실제 종료 여부와 관계없이 다음 노드가 실행되어 AI 상태 전이가 지연되거나 공격이 끊기는 문제가 발생했습니다.
* **해결 과정**:
  * `Wait` 노드를 제거했습니다.
  * **Behavior Tree Service**와 **Blackboard Decorator**를 활용하여 공격 가능 여부를 실시간으로 판단하도록 변경했습니다.
  * 공격 몽타주에 **C++ 기반 AnimNotify**를 추가하여 타격 시점과 애니메이션 종료 시점을 AI와 동기화했습니다.

* **결과**: Service와 Decorator를 통해 AI 상태를 실시간으로 판단하고, AnimNotify로 공격 종료 시점을 동기화하여 콤보 공격과 추격 상태가 자연스럽게 연결되는 AI 전투 흐름을 구현했습니다.


### 2. 무기 교체 시 왼손 IK 미동기화 문제 해결

* **문제 상황**: 무기 교체 시 캐릭터의 왼손이 총기 손잡이 위치와 맞지 않는 문제가 발생했습니다.
* **원인 분석**: 무기마다 메쉬의 크기와 형태가 다르지만, 애니메이션 자체는 고정되어 있어 런타임에서 무기별 파지 위치를 반영할 수 없었습니다.
* **해결 과정**:
  * 모든 무기 메쉬에 `LeftHandSocket` 을 추가했습니다.
  * Tick에서 매 프레임 무기 소켓의 Transform을 추출하여 AnimBP로 전달했습니다.
  * AnimBP에서 **Two Bone IK**를 이용해 왼손 본이 실시간으로 소켓 위치를 추적하도록 구현했습니다.
* **결과**: 무기 종류와 크기에 관계없이 **일관된 총기 파지 모션**을 유지할 수 있었습니다.

---

## 🚀 회고 및 성장

이번 프로젝트를 통해 Unreal Engine의 **Gameplay Framework**를 활용한 시스템 설계와 객체 간 의존성을 줄이는 방법을 경험했습니다. 특히 `ApplyPointDamage`를 활용한 피해 처리 구조와 `Behavior Tree`, `AnimNotify`를 연동한 AI 구현을 통해 엔진이 제공하는 프레임워크를 실제 프로젝트에 적용해 볼 수 있었습니다.

또한 현재는 무기 데이터를 C++ 구조체와 Blueprint에서 관리하고 있지만, 콘텐츠가 증가할수록 유지보수에 한계가 있을 것으로 판단했습니다. 앞으로는 **DataTable**과 **DataAsset**을 활용한 데이터 중심(Data-Driven) 구조를 적용하여 확장성과 유지보수성을 더욱 높여보고자 합니다.
