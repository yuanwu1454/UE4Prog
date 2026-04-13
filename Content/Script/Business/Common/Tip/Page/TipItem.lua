local TipItem = class("TipItem",PureMVC.GamePanel);

function TipItem:OnInitialized()
    TipItem.super.OnInitialized(self)
end
function TipItem:InitView(data)
    self._data = data
    self.Text_BlinkMsg:SetText(self._data.msg)
end

function TipItem:PlayShowAni()
    if self.ShowAnimation then
        self:PlayAnimation(self.ShowAnimation, 0, 1, UE4.EUMGSequencePlayMode.Forward, 1, false)
    end
end

return TipItem