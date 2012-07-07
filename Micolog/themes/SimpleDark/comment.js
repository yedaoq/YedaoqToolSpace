jQuery(document).ready(
function (){
	loadjs=false;	
	commentuser=$.cookie('comment_user');	
	if(commentuser)
	{
		//[user,email,url]=commentuser.split('#@#');
		data=commentuser.split('#@#');		
		$('#author').val(data[0]);		
		$('#email').val(data[1]);		
		$('#url').val(data[2]);		
	};	
	$('#commentform').ajaxForm({
		type:'post',
		dataType:'json',
		beforeSubmit:function (formData,jqForm,options){
			var form=jqForm[0];			
			if(form.author)
			{
				if(!form.author.value)
				{
					showinfo('请输入昵称!');					
					form.author.focus();					
					return false;					
				}
				if(!form.email.value)
				{
					showinfo('请输入邮箱地址!');					
					form.email.focus();					
					return false;					
				}
			}
			if($('#checkarea').css('display')=='block')
			{
				if(!form.checkret.value)
				{
					alert('请输入验证码!');					
					form.checkret.focus();					
					return false;					
				}
			}
			if(!form.comment.value)
			{
				showinfo('请输入评论内容!');				
				form.comment.focus();				
				return false;				
			}
			$('#s_msg').text('正在提交评论...').show();			
			$("#submit").attr('disabled',true);			
			return true;			
		},
		success:function (data){
			$("#submit").attr('disabled',false);			
			if(data[0])
			{
				/*document.cookie=data[2];*/
				alert('评论提交成功!');				
				add_comment(data[1]);				
				$('#s_msg').text('评论提交成功!');				
				$('#checkret').val('');				
				location="#comments";				
			}
			else 
			{
				if(data[1]==-102)
				showinfo('验证码错误。');				
				$('#s_msg').text('评论提交失败！');				
				$('#checkret').focus();				
			}
		}
	});	
}
)
function reloadCheckImage()
{
	var img=document.getElementById('checkimg');	
	img.src+="?";	
}
function showinfo(msg)
{
	alert(msg);	
}
function add_comment(msg)
{
	comment=$(msg)
	if(!loadjs)
	{
		$("#comment-list").prepend(comment).show();		
	}else 
	{
		$("#comment-list").prepend(comment);		
	}
}
function backcomment(name,id){
	backdb=document.getElementById('comment');	
	backdb.value+='<a href=\"#comment-'+id+'\">@'+name+':<\/a>'+'\n';	
	return true;	
}
function quote(name,id){
	var quoteMsg=document.getElementById(id).innerHTML;	
	document.getElementById("comment").value+='引用'+name+'<blockquote>'+quoteMsg+'</blockquote>\n';	
	return true;	
}
