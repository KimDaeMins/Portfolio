using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Photon.Pun;
using Photon.Realtime;

public class BloodDraw : MonoBehaviour
{
    [SerializeField] Texture2D Image;
    private Color drawColor = Color.red;
    private int brushSize = 30;
    private SpriteRenderer[] spriteRenderer;
    private BoxCollider2D collider;
   // private PhotonView _PV;
    List<Color[]> pixels = new List<Color[]>();
    List<Sprite> originalTextures = new List<Sprite>();
    List<Texture2D> textures = new List<Texture2D>();
    int pixelPerUnit = 0;
    [SerializeField] private int widthCount;
    [SerializeField] private int heightCount;
    HashSet<int> changed = new HashSet<int>();
    void Awake()
    {
       // _PV = GetComponentInParent<PhotonView>();
        spriteRenderer = GetComponentsInChildren<SpriteRenderer>();
        collider = GetComponent<BoxCollider2D>();
    }
    void Start()
    {
        for (int i = 0; i < spriteRenderer.Length; i++)
        {
            originalTextures.Add(spriteRenderer[i].sprite);
            Rect r = originalTextures[i].rect;
            textures.Add(new Texture2D((int)r.width, (int)r.height, TextureFormat.ARGB32, false));
            textures[i].SetPixels(originalTextures[i].texture.GetPixels((int)r.xMin, (int)r.yMin, (int)r.width, (int)r.height, 0));
            textures[i].Apply();
            pixels.Add(textures[i].GetPixels());
        }
        pixelPerUnit = (int)originalTextures[0].pixelsPerUnit;
        brushSize = 30;
    }

    private void OnParticleCollision(GameObject other)
    {
        //본체만 바꾸고 어찌저찌 해볼랬는데 뭔가 애매해서...
        ParticleSystem ptc = other.GetComponent<ParticleSystem>();
        List<ParticleCollisionEvent> collisionEvents = new List<ParticleCollisionEvent>();
        int numofCollision = ptc.GetCollisionEvents(this.gameObject, collisionEvents);
        bool pixelUpdate = false;
        changed.Clear();

        while (numofCollision > 0)
        {
            --numofCollision;
            RaycastHit2D hit = Physics2D.Raycast(collisionEvents[numofCollision].intersection,
                collisionEvents[numofCollision].velocity.normalized);
            if (hit)
            {
                if (hit.transform == transform)
                {
                    Vector2 localPos = hit.point - (Vector2)transform.position;
                    localPos = Quaternion.Inverse(transform.rotation) * localPos;
                    localPos.x -= collider.offset.x;
                    localPos.y -= collider.offset.y;

                    localPos.x = localPos.x / collider.size.x / transform.localScale.x + 0.5f;//여기가 UV
                    localPos.y = localPos.y / collider.size.y / transform.localScale.y + 0.5f;//여기가 UV

                    //여기까지 콜라이더 기준으로 UV를 짠거다.

                    //픽셀퍼유닛을 왜곱했더라? 컬러배열을 색칠할거여서...
                    localPos.x *= textures[0].width * widthCount;
                    localPos.y *= textures[0].height * heightCount;

                    pixelUpdate = DrawTexture(localPos);
                }
            }
        }
        if (pixelUpdate)
        {
            foreach (var val in changed)
            {
                textures[val].SetPixels(pixels[val]);
                textures[val].Apply();
                //_PV.RPC("UpdateSpriteRPC", RpcTarget.All, texture.GetRawTextureData());
                UpdateSpriteRPC(textures[val].GetRawTextureData(), val);
            }
        }
    }


    bool DrawTexture(Vector2 uv)
    {
        int x = (int)uv.x;
        int y = (int)uv.y;

        int startX = Mathf.Max(0, x - (int)(brushSize * transform.localScale.y / 2));
        int startY = Mathf.Max(0, y - (int)(brushSize * transform.localScale.x / 2));
        int endX = Mathf.Min(textures[0].width * widthCount, x + (int)(brushSize * transform.localScale.y / 2));
        int endY = Mathf.Min(textures[0].height * heightCount, y + (int)(brushSize * transform.localScale.x / 2));
        int longX = Mathf.Max(Mathf.Abs(startX - x), Mathf.Abs(endX - x));
        int longY = Mathf.Max(Mathf.Abs(startY - y), Mathf.Abs(endY - y));
        
        // ... 픽셀 변경 ...
        for (int i = startX; i < endX; i++)
        {
            for (int j = startY; j < endY; j++)
            {
                //현재i값 - x값의 절댓값
                int indexX = i % textures[0].width;
                int indexY = j % textures[0].height;
                int listIndex = i / textures[0].width + (j / textures[0].height) * widthCount;
                
                if (pixels[listIndex][indexY * textures[0].width + indexX].a > 0.2f &&
                (Random.Range(0, longX) - 2 >= Mathf.Abs(x - i)
                && Random.Range(0, longY) - 2 >= Mathf.Abs(y - j)))
                {
                    pixels[listIndex].SetValue(drawColor, indexY * textures[0].width + indexX);
                    changed.Add(listIndex);
                }
            }
        }
        return true;
    }

    //[PunRPC]
    private void UpdateSpriteRPC(byte[] receivedByte, int index)
    {
        textures[index].LoadRawTextureData(receivedByte);
        Vector2 middlePos = new Vector2(originalTextures[index].pivot.x / originalTextures[index].rect.width 
            , originalTextures[index].pivot.y / originalTextures[index].rect.height);
        spriteRenderer[index].sprite = Sprite.Create(textures[index], new Rect(0, 0, textures[index].width
            , textures[index].height), middlePos, originalTextures[index].pixelsPerUnit);
    }

}