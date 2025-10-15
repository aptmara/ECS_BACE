var searchData=
[
  ['define_5fbehaviour_0',['DEFINE_BEHAVIOUR',['../_component_samples_8h.html#a58cf298c54805477ad4f479e5c36b9c1',1,'DEFINE_BEHAVIOUR(SpinAndColor, float rotSpeed=90.0f;float colorSpeed=1.0f;float time=0.0f;, time+=dt *colorSpeed;auto *t=w.TryGet&lt; Transform &gt;(self);if(t) { t-&gt;rotation.y+=rotSpeed *dt;} auto *mr=w.TryGet&lt; MeshRenderer &gt;(self);if(mr) { float hue=fmodf(time, 1.0f);mr-&gt;color.x=sinf(hue *6.28f) *0.5f+0.5f;mr-&gt;color.y=cosf(hue *6.28f) *0.5f+0.5f;mr-&gt;color.z=0.5f;}):&#160;ComponentSamples.h'],['../_component_samples_8h.html#af0b48c402007c5ff2aee575426b1c2d7',1,'DEFINE_BEHAVIOUR(CircularMotion, float radius=3.0f;float speed=1.0f;float angle=0.0f;float centerY=0.0f;, angle+=speed *dt;auto *t=w.TryGet&lt; Transform &gt;(self);if(t) { t-&gt;position.x=cosf(angle) *radius;t-&gt;position.z=sinf(angle) *radius;t-&gt;position.y=centerY;}):&#160;ComponentSamples.h']]],
  ['define_5fdata_5fcomponent_1',['DEFINE_DATA_COMPONENT',['../_component_samples_8h.html#a050c78dba024b1abcefa3d69b4b79f36',1,'DEFINE_DATA_COMPONENT(Score, int points=0;void AddPoints(int p) { points+=p;} void Reset() { points=0;}):&#160;ComponentSamples.h'],['../_component_samples_8h.html#a016d79863b498bec0f5ec6ae8a3dc3ae',1,'DEFINE_DATA_COMPONENT(Name, const char *name=&quot;Unnamed&quot;;):&#160;ComponentSamples.h']]],
  ['destroyentity_2',['DestroyEntity',['../class_world.html#ae05a18cf43bde9b5d1b097c3fc1cc476',1,'World']]],
  ['dev_3',['Dev',['../class_gfx_device.html#a99d6cead18e76f4c4134c3a4a1000668',1,'GfxDevice']]],
  ['drawaxes_4',['DrawAxes',['../class_debug_draw.html#ae2214e1ec59a72886f6e237ff67dd990',1,'DebugDraw']]],
  ['drawgrid_5',['DrawGrid',['../class_debug_draw.html#a3220a1d6b9923874557f248ee76f5649',1,'DebugDraw']]]
];
