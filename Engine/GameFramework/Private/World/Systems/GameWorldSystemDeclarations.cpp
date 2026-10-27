#include "PCH.h"
#include "World/Systems/GameWorldSystems.h"

#include "World/Systems/Descriptors/GameWorldSystemContract.h"
#include "World/Systems/Execution/SystemChangeCommitter.h"
#include "World/Systems/GameSystemGraph.h"

namespace ECS
{
	GameSystemDesc GameWorldSystemExecution::DescribeCameraMovement()
	{
		GameSystemDesc camera{"Game.CameraMovement", GameSystemPhase::Simulation};
		camera.DeclareQuery<CameraMovementQuery>();

		camera.Resources = {
		    {GameSystemResourceDomain::UpdateInputs, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::CameraInputIntent, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::CameraNavigationSettings, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SystemChangeScratch, GameSystemAccessMode::Write}};

		camera.RangePolicy = GameWorldSystemGrain::Camera;

		camera.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_simulation.GetCameraCount();
		};

		camera.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_simulation.RunCamera(begin, end);
		};

		return camera;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeAnimationPlayback()
	{
		GameSystemDesc playback{"Game.AnimationPlaybackAdvance", GameSystemPhase::Animation};
		playback.DeclareQuery<PlaybackAdvanceQuery>();

		playback.Resources = {
		    {GameSystemResourceDomain::UpdateInputs, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::AnimationClips, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SystemChangeScratch, GameSystemAccessMode::Write}};

		playback.RangePolicy = GameWorldSystemGrain::Animation;

		playback.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_animation.GetPlaybackCount();
		};

		playback.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_animation.RunPlayback(begin, end);
		};

		return playback;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeAnimationPoseEvaluation(std::uint32_t playbackIndex)
	{
		GameSystemDesc pose{"Game.AnimationPoseEvaluation", GameSystemPhase::Animation};
		pose.DeclareQuery<PoseEvaluationQuery>();

		pose.Resources = {
		    {GameSystemResourceDomain::AnimationClips, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SkeletonResources, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::PoseScratch, GameSystemAccessMode::Write}};

		pose.Prerequisites = {playbackIndex};
		pose.RangePolicy = GameWorldSystemGrain::Pose;

		pose.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_animation.GetPoseCount();
		};

		pose.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_animation.RunPose(begin, end);
		};

		return pose;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeMorphWeightEvaluation(std::uint32_t playbackIndex)
	{
		GameSystemDesc morph{"Game.MorphWeightEvaluation", GameSystemPhase::Animation};
		morph.DeclareQuery<MorphEvaluationQuery>();
		morph.Resources = {{GameSystemResourceDomain::AnimationClips, GameSystemAccessMode::Read}, {GameSystemResourceDomain::MorphScratch, GameSystemAccessMode::Write}};
		morph.Prerequisites = {playbackIndex};
		morph.RangePolicy = GameWorldSystemGrain::Animation;

		morph.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_animation.GetMorphSampleCount();
		};

		morph.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_animation.RunMorphSamples(begin, end);
		};

		return morph;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeSkinningEvaluation(std::uint32_t poseIndex)
	{
		GameSystemDesc skinning{"Game.SkinningMatrixEvaluation", GameSystemPhase::Deformation};

		skinning.Resources = {
		    {GameSystemResourceDomain::PoseScratch, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SkeletonResources, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SkinningOutput, GameSystemAccessMode::Write}};

		skinning.Prerequisites = {poseIndex};
		skinning.RangePolicy = GameWorldSystemGrain::Pose;

		skinning.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_animation.GetSkinningCount();
		};

		skinning.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_animation.RunSkinning(begin, end);
		};

		return skinning;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeMorphOutputCommit(std::uint32_t morphIndex)
	{
		GameSystemDesc morphCommit{"Game.MorphOutputCommit", GameSystemPhase::Deformation};
		morphCommit.Resources = {{GameSystemResourceDomain::MorphScratch, GameSystemAccessMode::Read}, {GameSystemResourceDomain::MorphOutput, GameSystemAccessMode::Write}};
		morphCommit.Prerequisites = {morphIndex};
		morphCommit.RangePolicy = GameWorldSystemGrain::SingleItem;

		morphCommit.GetItemCount = [](GameWorldSystemExecution&)
		{
			return 1u;
		};

		morphCommit.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_animation.CommitMorphOutputs(begin, end);
		};

		return morphCommit;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeSystemOutputCommit(std::uint32_t skinningIndex, std::uint32_t morphCommitIndex)
	{
		GameSystemDesc outputCommit{"Game.SystemOutputCommit", GameSystemPhase::Deformation};

		outputCommit.Resources = {
		    {GameSystemResourceDomain::SkinningOutput, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::MorphOutput, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SystemChangeScratch, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::DirtyTransforms, GameSystemAccessMode::Write},
		    {GameSystemResourceDomain::WorldChanges, GameSystemAccessMode::Write}};

		outputCommit.Prerequisites = {skinningIndex, morphCommitIndex};
		outputCommit.RangePolicy = GameWorldSystemGrain::SingleItem;

		outputCommit.GetItemCount = [](GameWorldSystemExecution&)
		{
			return 1u;
		};

		outputCommit.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t, std::uint32_t)
		{
			return SystemChangeCommitter::CommitSystemOutputs(self.m_state);
		};

		return outputCommit;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeTransformEvaluation()
	{
		GameSystemDesc transform{"Game.TransformEvaluation", GameSystemPhase::Transform};
		transform.DeclareQuery<TransformEvaluationQuery>();
		transform.Resources = {{GameSystemResourceDomain::DirtyTransforms, GameSystemAccessMode::Read}, {GameSystemResourceDomain::TransformScratch, GameSystemAccessMode::Write}};
		transform.RangePolicy = GameWorldSystemGrain::Transform;

		transform.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_transform.GetDirtyTransformCount();
		};

		transform.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_transform.RunTransforms(begin, end);
		};

		return transform;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeCameraDerivedState(std::uint32_t transformIndex)
	{
		GameSystemDesc cameraDerived{"Game.CameraDerivedState", GameSystemPhase::Transform};
		cameraDerived.DeclareQuery<CameraDerivedStateQuery>();
		cameraDerived.Resources = {{GameSystemResourceDomain::DirtyTransforms, GameSystemAccessMode::Read}, {GameSystemResourceDomain::CameraDerivedScratch, GameSystemAccessMode::Write}};
		cameraDerived.Prerequisites = {transformIndex};
		cameraDerived.RangePolicy = GameWorldSystemGrain::Transform;

		cameraDerived.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_transform.GetDirtyTransformCount();
		};

		cameraDerived.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_transform.RunCameraDerivedState(begin, end);
		};

		return cameraDerived;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeMeshExtraction()
	{
		GameSystemDesc extraction{"Game.MeshExtraction", GameSystemPhase::Extraction};
		extraction.DeclareQuery<MeshExtractionQuery>();

		extraction.Resources = {
		    {GameSystemResourceDomain::MeshResources, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SkeletonResources, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::SkinningOutput, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::MorphOutput, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::ExtractionScratch, GameSystemAccessMode::Write}};

		extraction.RangePolicy = GameWorldSystemGrain::Extraction;

		extraction.GetItemCount = [](GameWorldSystemExecution& self)
		{
			return self.m_extraction.GetMeshCount();
		};

		extraction.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t begin, std::uint32_t end)
		{
			return self.m_extraction.Run(begin, end);
		};

		return extraction;
	}

	GameSystemDesc GameWorldSystemExecution::DescribeExtractionCommit(std::uint32_t extractionIndex)
	{
		GameSystemDesc extractionCommit{"Game.ExtractionCommit", GameSystemPhase::Extraction};

		extractionCommit.Resources = {
		    {GameSystemResourceDomain::ExtractionScratch, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::ExtractionOutput, GameSystemAccessMode::Write},
		    {GameSystemResourceDomain::TransformScratch, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::CameraDerivedScratch, GameSystemAccessMode::Read},
		    {GameSystemResourceDomain::WorldChanges, GameSystemAccessMode::Write},
		    {GameSystemResourceDomain::WorldPublication, GameSystemAccessMode::Write}};

		extractionCommit.Prerequisites = {extractionIndex};
		extractionCommit.RangePolicy = GameWorldSystemGrain::SingleItem;

		extractionCommit.GetItemCount = [](GameWorldSystemExecution&)
		{
			return 1u;
		};

		extractionCommit.ExecuteRange = [](GameWorldSystemExecution& self, std::uint32_t, std::uint32_t)
		{
			return SystemChangeCommitter::CommitExtraction(self.m_state);
		};

		return extractionCommit;
	}
}
